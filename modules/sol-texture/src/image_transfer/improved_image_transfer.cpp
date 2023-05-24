#include "sol-texture/image_transfer/improved_image_transfer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-error/sol_error.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/image2d.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ImprovedImageTransfer::ImprovedImageTransfer(MemoryManager& manager) : IImageTransfer(), memoryManager(&manager)
    {
        const auto& transferQueue  = memoryManager->getTransferQueue();
        const auto& transferFamily = transferQueue.getFamily();
        const auto& physicalDevice = memoryManager->getDevice().getPhysicalDevice();
        const auto  familyCount    = physicalDevice.getQueueFamilies().size();

        VulkanCommandBuffer::Settings cmdSettings;
        cmdSettings.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        VulkanSemaphore::Settings semSettings;
        semSettings.device = memoryManager->getDevice();
        VulkanFence::Settings fenceSettings;
        fenceSettings.device = memoryManager->getDevice();

        // Allocate resources for ownership transfers to the transfer queue.
        releaseCommandBuffers.reserve(familyCount);
        acquireCommandBuffers.reserve(familyCount);
        releaseSemaphores.reserve(familyCount);
        acquireFences.reserve(familyCount);
        for (uint32_t src = 0; src < familyCount; src++)
        {
            cmdSettings.commandPool = memoryManager->getCommandPool(src);
            releaseCommandBuffers.emplace_back(VulkanCommandBuffer::create(cmdSettings));
            acquireCommandBuffers.emplace_back(VulkanCommandBuffer::create(cmdSettings));

            releaseSemaphores.emplace_back(VulkanSemaphore::create(semSettings));
            acquireFences.emplace_back(VulkanFence::create(fenceSettings));
        }

        // Allocate resources for copying.
        cmdSettings.commandPool = memoryManager->getCommandPool(transferFamily);
        copy.commandBuffer      = VulkanCommandBuffer::create(cmdSettings);
        copy.fence              = VulkanFence::create(fenceSettings);

        // Allocate resources for layout transitions and ownership transfers.
        commands.reserve(familyCount * familyCount);
        for (uint32_t dst = 0; dst < familyCount; dst++)
        {
            for (uint32_t src = 0; src < familyCount; src++)
            {
                auto& cmd = commands.emplace_back(&physicalDevice.getQueueFamilies()[src],
                                                  &physicalDevice.getQueueFamilies()[dst],
                                                  src == dst ? nullptr : VulkanSemaphore::create(semSettings),
                                                  VulkanFence::create(fenceSettings));

                cmdSettings.commandPool = memoryManager->getCommandPool(src);
                cmd.srcCommandBuffer    = VulkanCommandBuffer::create(cmdSettings);
                cmdSettings.commandPool = memoryManager->getCommandPool(dst);
                cmd.dstCommandBuffer    = VulkanCommandBuffer::create(cmdSettings);
            }
        }
    }

    ImprovedImageTransfer::~ImprovedImageTransfer() noexcept { wait(); }

    ////////////////////////////////////////////////////////////////
    // Transfer.
    ////////////////////////////////////////////////////////////////

    size_t ImprovedImageTransfer::createStagingBuffer(Image2D&                       image,
                                                      const std::array<uint32_t, 2>& regionOffset,
                                                      const std::array<uint32_t, 2>& regionSize)
    {
        // It is assumed here that regionOffset and regionSize are valid because they were checked in e.g. the Image2D methods.

        // Check region does not overlap any previous regions.
        auto it = staged.find(&image);
        if (it != staged.end())
        {
            for (const auto& buffer : it->second.copies)
            {
                const std::array lower0 = regionOffset;
                const std::array upper0 = {lower0[0] + regionSize[0], lower0[1] + regionSize[1]};
                const std::array lower1 = buffer.regionOffset;
                const std::array upper1 = {lower1[0] + buffer.regionSize[0], lower1[1] + buffer.regionSize[1]};
                if (overlaps(lower0, upper0, lower1, upper1))
                    throw SolError("Cannot create image staging buffer for overlapping regions.");
            }
        }

        // TODO: This should not be derived from the memory requirements, as the size could be larger due to alignment.
        // Instead, it should be derived from the VkFormat. Vulkan ValidationLayers has some generated code that does this:
        // https://github.com/KhronosGroup/Vulkan-ValidationLayers/blob/master/layers/generated/vk_format_utils.h
        const auto perPixelSize =
          image.getImage().getMemoryRequirements().size / (static_cast<size_t>(image.getWidth()) * image.getHeight());
        const auto bufferSize = perPixelSize * regionSize[0] * regionSize[1];

        // Create persistently mapped staging buffer.
        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device      = memoryManager->getDevice();
        bufferSettings.bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferSettings.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferSettings.allocator   = memoryManager->getAllocator();
        bufferSettings.memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
        bufferSettings.flags       = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        bufferSettings.size        = bufferSize;
        auto buffer                = VulkanBuffer::create(bufferSettings);
        buffer->map();  //TODO: This map necessary?

        // Create new list of StagingBuffers for this image.
        if (it == staged.end())
        {
            it                = staged.try_emplace(&image).first;
            it->second.image  = image.getImage().get();
            it->second.aspect = image.getAspectFlags();
        }

        // Store staging buffer.
        it->second.copies.emplace_back(
          CopyInfo{.buffer = std::move(buffer), .regionOffset = regionOffset, .regionSize = regionSize});

        return it->second.copies.size() - 1;
    }

    ImageStagingBuffer ImprovedImageTransfer::getStagingBuffer(Image2D& image, const size_t index)
    {
        const auto it = staged.find(&image);
        if (it == staged.end())
            throw SolError("Cannot get staging buffer. No staging buffer was created for this image.");
        if (index >= it->second.copies.size()) throw SolError("Cannot get staging buffer. Index out of bounds.");

        const auto& [buffer, regionOffset, regionSize] = it->second.copies[index];
        return {*buffer, regionOffset, regionSize};
    }

    void ImprovedImageTransfer::stageTransition(Image2D&                           image,
                                                const VulkanQueueFamily*           queueFamily,
                                                const std::optional<VkImageLayout> imageLayout,
                                                const VkPipelineStageFlags2        srcStage,
                                                const VkPipelineStageFlags2        dstStage,
                                                const VkAccessFlags2               srcAccess,
                                                const VkAccessFlags2               dstAccess)
    {
        // Get or create existing info object.
        TransferInfo& info = staged.try_emplace(&image).first->second;

        info.image                = image.getImage().get();
        info.aspect               = image.getAspectFlags();
        info.transition.srcStage  = srcStage;
        info.transition.dstStage  = dstStage;
        info.transition.srcAccess = srcAccess;
        info.transition.dstAccess = dstAccess;
        info.transition.srcFamily = image.getQueueFamily();
        info.transition.dstFamily = queueFamily;

        if (!info.transition.srcFamily && !info.transition.dstFamily)
            throw SolError("Cannot stage Image2D transition without source and target queue family.");

        if (imageLayout)
        {
            info.transition.oldLayout = image.getImageLayout();
            info.transition.newLayout = *imageLayout;
        }
    }

    void ImprovedImageTransfer::transfer()
    {
        if (staged.empty())
        {
            wait();
            return;
        }

        prepareCopy();
        copyImages();
        transition();

        // Keep track of staged copies. They are currently being processed
        // and staging buffers should not be deallocated until next transfer.
        pending = std::move(staged);
    }

    void ImprovedImageTransfer::prepareCopy()
    {
        auto&        device         = memoryManager->getDevice();
        auto&        physicalDevice = device.getPhysicalDevice();
        const size_t familyCount    = physicalDevice.getQueueFamilies().size();
        auto&        transferQueue  = memoryManager->getTransferQueue();

        //
        std::vector<std::vector<VkImageMemoryBarrier2>> releaseBarriers(familyCount);
        std::vector<std::vector<VkImageMemoryBarrier2>> acquireBarriers(familyCount);
        for (const auto& info : staged | std::views::values)
        {
            // Skip if there are no copies needed.
            if (info.copies.empty()) continue;

            VkImageMemoryBarrier2 barrier{};
            barrier.sType        = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;  // TODO: Should we use the user supplied stage
            barrier.srcAccessMask =
              VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;  // and access masks here, assuming they are set?
            barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            barrier.dstAccessMask                   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier.oldLayout                       = info.transition.oldLayout;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.image                           = info.image;
            barrier.subresourceRange.aspectMask     = info.aspect;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount     = 1;


            // If the image has an owner that is not the transfer queue, a barrier on both queues is needed.
            if (info.transition.srcFamily && info.transition.srcFamily != &transferQueue.getFamily())
            {
                barrier.srcQueueFamilyIndex = info.transition.srcFamily->getIndex();
                barrier.dstQueueFamilyIndex = transferQueue.getFamily().getIndex();
                releaseBarriers[info.transition.srcFamily->getIndex()].emplace_back(barrier);
            }
            // Otherwise, only an acquire barrier on the transfer queue is needed.
            else
            {
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            }

            acquireBarriers[transferQueue.getFamily().getIndex()].emplace_back(barrier);
        }

        // Wait for previous submits to complete before resetting and recording command buffers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (acquireFences[i]->isSignaled())
            {
                vkWaitForFences(device.get(), 1, &acquireFences[i]->get(), true, UINT64_MAX);
                vkResetFences(device.get(), 1, &acquireFences[i]->get());
                acquireFences[i]->setSignaled(false);
            }
        }

        //
        for (uint32_t i = 0; i < familyCount; i++)
        {
            // Write the release command.
            if (!releaseBarriers[i].empty())
            {
                VkDependencyInfo info{};
                info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
                auto& cb   = *releaseCommandBuffers[i];
                cb.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                cb.beginOneTimeCommand();
                info.imageMemoryBarrierCount = static_cast<uint32_t>(releaseBarriers[i].size());
                info.pImageMemoryBarriers    = releaseBarriers[i].data();
                vkCmdPipelineBarrier2(cb.get(), &info);
                cb.endCommand();
            }

            // Write the acquire command.
            if (!acquireBarriers[i].empty())
            {
                VkDependencyInfo info{};
                info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
                auto& cb   = *acquireCommandBuffers[i];
                cb.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                cb.beginOneTimeCommand();
                info.imageMemoryBarrierCount = static_cast<uint32_t>(acquireBarriers.size());
                info.pImageMemoryBarriers    = acquireBarriers[i].data();
                vkCmdPipelineBarrier2(cb.get(), &info);
                cb.endCommand();
            }
        }

        //
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (!releaseBarriers[i].empty())
            {
                VkSubmitInfo submit{};
                submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit.pNext              = nullptr;
                submit.waitSemaphoreCount = 0;
                submit.pWaitSemaphores    = nullptr;
                submit.pWaitDstStageMask  = nullptr;
                submit.commandBufferCount = 1;
                submit.pCommandBuffers    = &releaseCommandBuffers[i]->get();
                // Semaphore is only needed when transferring between queues.
                if (i != transferQueue.getFamily().getIndex())
                {
                    submit.signalSemaphoreCount = 1;
                    submit.pSignalSemaphores    = &releaseSemaphores[i]->get();
                }
                memoryManager->getQueue(i).submit(submit);
            }

            if (!acquireBarriers[i].empty())
            {
                VkSubmitInfo submit{};
                submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit.pNext = nullptr;
                // Semaphore is only needed when transferring between queues.
                if (i != transferQueue.getFamily().getIndex())
                {
                    submit.waitSemaphoreCount             = 1;
                    submit.pWaitSemaphores                = &releaseSemaphores[i]->get();
                    VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                    submit.pWaitDstStageMask              = &waitDstStageMask;
                }
                submit.commandBufferCount   = 1;
                submit.pCommandBuffers      = &acquireCommandBuffers[i]->get();
                submit.signalSemaphoreCount = 0;
                submit.pSignalSemaphores    = nullptr;
                transferQueue.submit(submit, acquireFences[i]->get());
                acquireFences[i]->setSignaled(true);
            }
        }
    }

    void ImprovedImageTransfer::copyImages()
    {
        auto& transferQueue = memoryManager->getTransferQueue();

        // Wait for previous call to this method to complete before clearing command buffer.
        wait();

        // TODO: This does a redundant submit when there are no copies.
        copy.commandBuffer->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        copy.commandBuffer->beginOneTimeCommand();

        for (const auto& info : staged | std::views::values)
        {
            for (const auto& [stagingBuffer, regionOffset, regionSize] : info.copies)
            {
                VkBufferImageCopy region;
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = 0;
                region.bufferImageHeight               = 0;
                region.imageSubresource.aspectMask     = info.aspect;
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset.x                   = static_cast<int32_t>(regionOffset[0]);
                region.imageOffset.y                   = static_cast<int32_t>(regionOffset[1]);
                region.imageOffset.z                   = 0;
                region.imageExtent.width               = regionSize[0];
                region.imageExtent.height              = regionSize[1];
                region.imageExtent.depth               = 1;
                vkCmdCopyBufferToImage(copy.commandBuffer->get(),
                                       stagingBuffer->get(),
                                       info.image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       1,
                                       &region);
            }
        }

        copy.commandBuffer->endCommand();

        VkSubmitInfo submitInfo;
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext                = nullptr;
        submitInfo.waitSemaphoreCount   = 0;
        submitInfo.pWaitSemaphores      = nullptr;
        submitInfo.pWaitDstStageMask    = nullptr;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &copy.commandBuffer->get();
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores    = nullptr;
        transferQueue.submit(submitInfo, copy.fence->get());
        copy.fence->setSignaled(true);
    }

    void ImprovedImageTransfer::transition()
    {
        auto&        device         = memoryManager->getDevice();
        auto&        physicalDevice = device.getPhysicalDevice();
        const size_t familyCount    = physicalDevice.getQueueFamilies().size();
        auto&        transferQueue  = memoryManager->getTransferQueue();

        const auto calcIdx = [familyCount](const uint32_t src, const uint32_t dst) {
            if (src == VK_QUEUE_FAMILY_IGNORED) return dst + dst * familyCount;
            if (dst == VK_QUEUE_FAMILY_IGNORED) return src + src * familyCount;
            return src + dst * familyCount;
        };

        std::vector<std::vector<VkImageMemoryBarrier2>> releaseBarriers(familyCount * familyCount);
        std::vector<std::vector<VkImageMemoryBarrier2>> acquireBarriers(familyCount * familyCount);
        for (const auto& [image, info] : staged)
        {
            image->setImageLayout(info.transition.newLayout);
            if (info.transition.dstFamily) image->setQueueFamily(*info.transition.dstFamily);

            VkImageMemoryBarrier2 barrier{};
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.pNext                           = nullptr;
            barrier.image                           = info.image;
            barrier.subresourceRange.aspectMask     = info.aspect;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount     = 1;
            barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;

            if (info.copies.empty())
            {
                barrier.srcStageMask  = info.transition.srcStage;
                barrier.srcAccessMask = info.transition.srcAccess;
                barrier.dstStageMask  = info.transition.dstStage;
                barrier.dstAccessMask = info.transition.dstAccess;
                barrier.oldLayout     = info.transition.oldLayout;
                barrier.newLayout     = info.transition.newLayout;

                if (info.transition.srcFamily && info.transition.dstFamily)
                {
                    // Ownership transfer on the same queue was requested. Only acquire on the one queue is needed.
                    if (info.transition.srcFamily == info.transition.dstFamily)
                    {
                        const auto idx = calcIdx(info.transition.srcFamily->getIndex(), VK_QUEUE_FAMILY_IGNORED);
                        acquireBarriers[idx].emplace_back(barrier);
                    }
                    // Ownership transfer from one queue to another. Release and acquire needed.
                    else
                    {
                        barrier.srcQueueFamilyIndex = info.transition.srcFamily->getIndex();
                        barrier.dstQueueFamilyIndex = info.transition.dstFamily->getIndex();
                        const auto idx              = calcIdx(barrier.srcQueueFamilyIndex, barrier.dstQueueFamilyIndex);
                        releaseBarriers[idx].emplace_back(barrier);
                        acquireBarriers[idx].emplace_back(barrier);
                    }
                }
                // No ownership transfer was requested. Only acquire on the same queue is needed.
                else if (info.transition.srcFamily)
                {
                    const auto idx = calcIdx(info.transition.srcFamily->getIndex(), VK_QUEUE_FAMILY_IGNORED);
                    acquireBarriers[idx].emplace_back(barrier);
                }
                // Image is not yet owned by any family. Only acquire on the destination queue is needed.
                else if (info.transition.dstFamily)
                {
                    const auto idx = calcIdx(VK_QUEUE_FAMILY_IGNORED, info.transition.dstFamily->getIndex());
                    acquireBarriers[idx].emplace_back(barrier);
                }
                else { assert(false); }
            }
            else
            {
                barrier.srcStageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                barrier.dstStageMask  = info.transition.dstStage;
                barrier.dstAccessMask = info.transition.dstAccess;
                barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout     = info.transition.newLayout;

                // Ownership transfer to queue different from transfer queue was requested. Release on transfer queue, acquire on destination queue.
                if (info.transition.dstFamily && info.transition.dstFamily != &transferQueue.getFamily())
                {
                    barrier.srcQueueFamilyIndex = transferQueue.getFamily().getIndex();
                    barrier.dstQueueFamilyIndex = info.transition.dstFamily->getIndex();
                    const auto idx              = calcIdx(barrier.srcQueueFamilyIndex, barrier.dstQueueFamilyIndex);
                    releaseBarriers[idx].emplace_back(barrier);
                    acquireBarriers[idx].emplace_back(barrier);
                }
                // No ownership transfer was requested. Only acquire on the transfer queue is needed.
                else
                {
                    const auto idx = calcIdx(VK_QUEUE_FAMILY_IGNORED, transferQueue.getFamily().getIndex());
                    acquireBarriers[idx].emplace_back(barrier);
                }
            }
        }

        // Wait for previous submits to complete before resetting and recording command buffers.
        for (uint32_t i = 0; i < familyCount * familyCount; i++)
        {
            if (commands[i].fence->isSignaled())
            {
                vkWaitForFences(device.get(), 1, &commands[i].fence->get(), true, UINT64_MAX);
                vkResetFences(device.get(), 1, &commands[i].fence->get());
                commands[i].fence->setSignaled(false);
            }
        }

        //
        for (uint32_t dst = 0; dst < familyCount; dst++)
        {
            for (uint32_t src = 0; src < familyCount; src++)
            {
                const auto idx = calcIdx(src, dst);
                if (releaseBarriers[idx].empty() && acquireBarriers[idx].empty()) continue;

                auto&            cmd = commands[idx];
                VkDependencyInfo info{};
                info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

                if (!releaseBarriers[idx].empty())
                {
                    cmd.srcCommandBuffer->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                    cmd.srcCommandBuffer->beginOneTimeCommand();
                    info.imageMemoryBarrierCount = static_cast<uint32_t>(releaseBarriers[idx].size());
                    info.pImageMemoryBarriers    = releaseBarriers[idx].data();
                    vkCmdPipelineBarrier2(cmd.srcCommandBuffer->get(), &info);
                    cmd.srcCommandBuffer->endCommand();
                }

                if (!acquireBarriers[idx].empty())
                {
                    cmd.dstCommandBuffer->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                    cmd.dstCommandBuffer->beginOneTimeCommand();
                    info.imageMemoryBarrierCount = static_cast<uint32_t>(acquireBarriers[idx].size());
                    info.pImageMemoryBarriers    = acquireBarriers[idx].data();
                    vkCmdPipelineBarrier2(cmd.dstCommandBuffer->get(), &info);
                    cmd.dstCommandBuffer->endCommand();
                }
            }
        }

        //
        for (uint32_t dst = 0; dst < familyCount; dst++)
        {
            for (uint32_t src = 0; src < familyCount; src++)
            {
                const auto idx = calcIdx(src, dst);
                if (releaseBarriers[idx].empty() && acquireBarriers[idx].empty()) continue;

                auto& cmd = commands[idx];

                if (!releaseBarriers[idx].empty())
                {
                    VkSubmitInfo submit{};
                    submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    submit.pNext              = nullptr;
                    submit.waitSemaphoreCount = 0;
                    submit.pWaitSemaphores    = nullptr;
                    submit.pWaitDstStageMask  = nullptr;
                    submit.commandBufferCount = 1;
                    submit.pCommandBuffers    = &cmd.srcCommandBuffer->get();
                    // Semaphore is only needed when transferring between queues.
                    if (src != dst)
                    {
                        submit.signalSemaphoreCount = 1;
                        submit.pSignalSemaphores    = &cmd.sem->get();
                    }
                    memoryManager->getQueue(src).submit(submit);
                }

                if (!acquireBarriers[idx].empty())
                {
                    VkSubmitInfo submit{};
                    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    submit.pNext = nullptr;
                    // Semaphore is only needed when transferring between queues.
                    if (src != dst)
                    {
                        submit.waitSemaphoreCount             = 1;
                        submit.pWaitSemaphores                = &cmd.sem->get();
                        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                        submit.pWaitDstStageMask              = &waitDstStageMask;
                    }
                    submit.commandBufferCount   = 1;
                    submit.pCommandBuffers      = &cmd.dstCommandBuffer->get();
                    submit.signalSemaphoreCount = 0;
                    submit.pSignalSemaphores    = nullptr;
                    memoryManager->getQueue(dst).submit(submit, cmd.fence->get());
                    cmd.fence->setSignaled(true);
                }
            }
        }
    }

    void ImprovedImageTransfer::wait()
    {
        // Wait for fence that signals completion of the copies.
        if (copy.fence->isSignaled())
        {
            const auto& device = memoryManager->getDevice();
            vkWaitForFences(device.get(), 1, &copy.fence->get(), true, UINT64_MAX);
            vkResetFences(device.get(), 1, &copy.fence->get());
            copy.fence->setSignaled(false);
        }

        // Clear pending copies after fence so that staging buffers can be deallocated.
        for (const auto& info : pending | std::views::values)
            for (const auto& c : info.copies) c.buffer->unmap();
        pending.clear();
    }
}  // namespace sol
