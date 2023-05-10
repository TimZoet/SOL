#include "sol-texture/image_transfer/default_image_transfer.h"

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
#include "sol-core/vulkan_command_pool.h"
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

    DefaultImageTransfer::DefaultImageTransfer(MemoryManager& manager) : IImageTransfer(), memoryManager(&manager)
    {
        const auto& transferQueue  = memoryManager->getTransferQueue();
        auto*       transferFamily = &transferQueue.getFamily();
        auto&       physicalDevice = memoryManager->getDevice().getPhysicalDevice();
        const auto  size           = physicalDevice.getQueueFamilies().size();

        for (uint32_t i = 0; i < size; i++)
        {
            auto& commandPool         = memoryManager->getCommandPool(i);
            auto& transferCommandPool = memoryManager->getCommandPool(*transferFamily);
            acquireState.releaseCommandBuffers.emplace_back(
              commandPool.createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));
            acquireState.acquireCommandBuffers.emplace_back(
              transferCommandPool.createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));
            releaseState.releaseCommandBuffers.emplace_back(
              transferCommandPool.createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));
            releaseState.acquireCommandBuffers.emplace_back(
              commandPool.createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY));
        }

        copyState.commandBuffer =
          memoryManager->getCommandPool(*transferFamily).createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VulkanSemaphore::Settings semSettings;
        semSettings.device      = memoryManager->getDevice();
        acquireState.semaphores = VulkanSemaphore::create(semSettings, size);
        releaseState.semaphores = VulkanSemaphore::create(semSettings, size);

        VulkanFence::Settings fenceSettings;
        fenceSettings.device = memoryManager->getDevice();
        acquireState.fence   = VulkanFence::create(fenceSettings);
        copyState.fence      = VulkanFence::create(fenceSettings);
        releaseState.fences  = VulkanFence::create(fenceSettings, size);
    }

    DefaultImageTransfer::~DefaultImageTransfer() noexcept { wait(); }

    ////////////////////////////////////////////////////////////////
    // Transfer.
    ////////////////////////////////////////////////////////////////

    size_t DefaultImageTransfer::createStagingBuffer(Image2D&                       image,
                                                     const std::array<uint32_t, 2>& regionOffset,
                                                     const std::array<uint32_t, 2>& regionSize)
    {
        // It is assumed here that regionOffset and regionSize are valid because they were checked in e.g. the Image2D methods.

        // Check region does not overlap any previous regions.
        auto it = stagedCopies.find(&image);
        if (it != stagedCopies.end())
        {
            for (const auto& buffer : it->second)
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
        if (it == stagedCopies.end()) it = stagedCopies.try_emplace(&image).first;

        // Store staging buffer.
        it->second.emplace_back(
          StagingBuffer{.stagingBuffer = std::move(buffer), .regionOffset = regionOffset, .regionSize = regionSize});

        return it->second.size() - 1;
    }

    ImageStagingBuffer DefaultImageTransfer::getStagingBuffer(Image2D& image, const size_t index)
    {
        const auto it = stagedCopies.find(&image);
        if (it == stagedCopies.end())
            throw SolError("Cannot get staging buffer. No staging buffer was created for this image.");
        if (index >= it->second.size()) throw SolError("Cannot get staging buffer. Index out of bounds.");

        auto& stagingBuffer = it->second[index];
        return ImageStagingBuffer(*stagingBuffer.stagingBuffer, stagingBuffer.regionOffset, stagingBuffer.regionSize);
    }

    void DefaultImageTransfer::stageLayoutTransition(Image2D& image)
    {
        // Add image with empty vector. If image was already staged, this will do nothing.
        stagedCopies.try_emplace(&image);
    }

    void DefaultImageTransfer::stageOwnershipTransfer(Image2D& image)
    {
        // Add image with empty vector. If image was already staged, this will do nothing.
        stagedCopies.try_emplace(&image);
    }

    void DefaultImageTransfer::transfer()
    {
        if (stagedCopies.empty())
        {
            wait();
            return;
        }

        acquireOwnership();
        copyImages();
        releaseOwnership();

        // Keep track of staged copies. They are currently being processed
        // and staging buffers should not be deallocated until next transfer.
        pendingCopies = std::move(stagedCopies);
    }

    void DefaultImageTransfer::acquireOwnership()
    {
        auto&        device         = memoryManager->getDevice();
        auto&        physicalDevice = device.getPhysicalDevice();
        const size_t familyCount    = physicalDevice.getQueueFamilies().size();
        auto&        transferQueue  = memoryManager->getTransferQueue();
        auto*        transferFamily = &transferQueue.getFamily();

        // Pre-initialize with shared default values.
        VkImageMemoryBarrier barrier;
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext                           = nullptr;
        barrier.srcAccessMask                   = 0;
        barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        std::vector<std::vector<VkImageMemoryBarrier>> releaseBarriers(familyCount);
        std::vector<std::vector<VkImageMemoryBarrier>> acquireBarriers(familyCount);

        for (const auto& [image, copies] : stagedCopies)
        {
            // If there are no copies for this image, it only needs an ownership or layout transfer. This is handled in the release.
            if (copies.empty()) continue;

            barrier.image                       = image->getImage().get();
            barrier.subresourceRange.aspectMask = image->getAspectFlags();

            // If the image has an owner that is not the transfer queue, a barrier on both queues is needed.
            if (image->getCurrentFamily() && image->getCurrentFamily() != transferFamily)
            {
                const auto familyIndex      = image->getCurrentFamily()->getIndex();
                barrier.oldLayout           = image->getCurrentLayout();
                barrier.srcQueueFamilyIndex = familyIndex;
                barrier.dstQueueFamilyIndex = transferFamily->getIndex();

                releaseBarriers[familyIndex].emplace_back(barrier);
                acquireBarriers[familyIndex].emplace_back(barrier);
            }
            // If the image does not yet have an owner or is already owned by the transfer queue, only an acquire barrier on the transfer queue is needed.
            else
            {
                barrier.oldLayout           = image->getCurrentLayout();
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                acquireBarriers[transferFamily->getIndex()].emplace_back(barrier);
            }
        }

        // Wait for previous call to this method to complete before resetting and recording command buffers.
        if (acquireState.fence->isSignaled())
        {
            vkWaitForFences(device.get(), 1, &acquireState.fence->get(), true, UINT64_MAX);
            vkResetFences(device.get(), 1, &acquireState.fence->get());
            acquireState.fence->setSignaled(false);
        }

        // Record pipeline barrier commands.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (acquireBarriers[i].empty()) continue;

            if (i != transferFamily->getIndex())
            {
                auto& releaseCmd = *acquireState.releaseCommandBuffers[i];
                releaseCmd.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                releaseCmd.beginOneTimeCommand();

                vkCmdPipelineBarrier(releaseCmd.get(),
                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     0,
                                     0,
                                     nullptr,
                                     0,
                                     nullptr,
                                     static_cast<uint32_t>(releaseBarriers[i].size()),
                                     releaseBarriers[i].data());

                releaseCmd.endCommand();
            }

            {
                auto& acquireCmd = *acquireState.acquireCommandBuffers[i];
                acquireCmd.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                acquireCmd.beginOneTimeCommand();

                vkCmdPipelineBarrier(acquireCmd.get(),
                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     0,
                                     0,
                                     nullptr,
                                     0,
                                     nullptr,
                                     static_cast<uint32_t>(acquireBarriers[i].size()),
                                     acquireBarriers[i].data());

                acquireCmd.endCommand();
            }
        }

        std::vector<VkSubmitInfo> acquireSubmits;

        // Submit pipeline barriers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (acquireBarriers[i].empty()) continue;

            if (i != transferFamily->getIndex())
            {
                // Submits on different queues require a semaphore.
                const auto& semaphore = *acquireState.semaphores[i];

                // Submit release to source queue.
                VkSubmitInfo releaseSubmit{};
                releaseSubmit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                releaseSubmit.pNext                = nullptr;
                releaseSubmit.waitSemaphoreCount   = 0;
                releaseSubmit.pWaitSemaphores      = nullptr;
                releaseSubmit.pWaitDstStageMask    = nullptr;
                releaseSubmit.commandBufferCount   = 1;
                releaseSubmit.pCommandBuffers      = &acquireState.releaseCommandBuffers[i]->get();
                releaseSubmit.signalSemaphoreCount = 1;
                releaseSubmit.pSignalSemaphores    = &semaphore.get();
                memoryManager->getQueue(i).submit(releaseSubmit);

                // Aggregate acquire submits to transfer queue.
                VkSubmitInfo acquireSubmit{};
                acquireSubmit.sType                   = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                acquireSubmit.pNext                   = nullptr;
                acquireSubmit.waitSemaphoreCount      = 1;
                acquireSubmit.pWaitSemaphores         = &semaphore.get();
                VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                acquireSubmit.pWaitDstStageMask       = &waitDstStageMask;
                acquireSubmit.commandBufferCount      = 1;
                acquireSubmit.pCommandBuffers         = &acquireState.acquireCommandBuffers[i]->get();
                acquireSubmit.signalSemaphoreCount    = 0;
                acquireSubmit.pSignalSemaphores       = nullptr;
                acquireSubmits.emplace_back(acquireSubmit);
            }
            else
            {
                // Aggregate acquire submits to transfer queue.
                VkSubmitInfo acquireSubmit{};
                acquireSubmit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                acquireSubmit.pNext                = nullptr;
                acquireSubmit.waitSemaphoreCount   = 0;
                acquireSubmit.pWaitSemaphores      = nullptr;
                acquireSubmit.pWaitDstStageMask    = nullptr;
                acquireSubmit.commandBufferCount   = 1;
                acquireSubmit.pCommandBuffers      = &acquireState.acquireCommandBuffers[i]->get();
                acquireSubmit.signalSemaphoreCount = 0;
                acquireSubmit.pSignalSemaphores    = nullptr;
                acquireSubmits.emplace_back(acquireSubmit);
            }
        }

        // Acquire submits are on the same queue, so they can be submitted together.
        transferQueue.submit(
          acquireSubmits.data(), static_cast<uint32_t>(acquireSubmits.size()), acquireState.fence->get());

        acquireState.fence->setSignaled(true);
    }

    void DefaultImageTransfer::copyImages()
    {
        auto& transferQueue = memoryManager->getTransferQueue();

        // Wait for previous call to this method to complete before clearing command buffer.
        wait();

        copyState.commandBuffer->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        copyState.commandBuffer->beginOneTimeCommand();

        for (const auto& [image, stagingBuffers] : stagedCopies)
        {
            for (const auto& [stagingBuffer, regionOffset, regionSize] : stagingBuffers)
            {
                VkBufferImageCopy region;
                region.bufferOffset                    = 0;
                region.bufferRowLength                 = 0;
                region.bufferImageHeight               = 0;
                region.imageSubresource.aspectMask     = image->getAspectFlags();
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageOffset.x                   = static_cast<int32_t>(regionOffset[0]);
                region.imageOffset.y                   = static_cast<int32_t>(regionOffset[1]);
                region.imageOffset.z                   = 0;
                region.imageExtent.width               = regionSize[0];
                region.imageExtent.height              = regionSize[1];
                region.imageExtent.depth               = 1;
                vkCmdCopyBufferToImage(copyState.commandBuffer->get(),
                                       stagingBuffer->get(),
                                       image->getImage().get(),
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       1,
                                       &region);
            }
        }

        copyState.commandBuffer->endCommand();

        VkSubmitInfo submitInfo;
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext                = nullptr;
        submitInfo.waitSemaphoreCount   = 0;
        submitInfo.pWaitSemaphores      = nullptr;
        submitInfo.pWaitDstStageMask    = nullptr;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &copyState.commandBuffer->get();
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores    = nullptr;
        transferQueue.submit(submitInfo, copyState.fence->get());
        copyState.fence->setSignaled(true);
    }

    void DefaultImageTransfer::releaseOwnership()
    {
        auto&        device         = memoryManager->getDevice();
        auto&        physicalDevice = device.getPhysicalDevice();
        const size_t familyCount    = physicalDevice.getQueueFamilies().size();
        auto&        transferQueue  = memoryManager->getTransferQueue();
        auto*        transferFamily = &transferQueue.getFamily();

        VkImageSubresourceRange range;
        range.baseMipLevel   = 0;
        range.levelCount     = 1;
        range.baseArrayLayer = 0;
        range.layerCount     = 1;

        VkImageMemoryBarrier barrier;
        barrier.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext            = nullptr;
        barrier.oldLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.subresourceRange = range;

        std::vector<std::vector<VkImageMemoryBarrier>> releaseBarriers(familyCount);
        std::vector<std::vector<VkImageMemoryBarrier>> acquireBarriers(familyCount);
        std::vector<VkPipelineStageFlags>              acquireDstStageMasks(familyCount, 0);

        for (auto& [image, copies] : stagedCopies)
        {
            barrier.image                       = image->getImage().get();
            barrier.subresourceRange.aspectMask = image->getAspectFlags();

            // If there are no copies for this image, it only needs an ownership transfer and/or layout transition.
            if (copies.empty())
            {
                const auto familyIndex = image->getTargetFamily()->getIndex();
                barrier.srcAccessMask  = 0;
                barrier.dstAccessMask  = image->getAccessFlags();
                barrier.oldLayout      = image->getCurrentLayout();
                barrier.newLayout      = image->getTargetLayout();

                // Image has no current family or only needs a layout transition. Only an acquire on the target queue is needed.
                if (image->getCurrentFamily() == nullptr || image->getCurrentFamily() == image->getTargetFamily())
                {
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                    acquireBarriers[familyIndex].emplace_back(barrier);
                }
                // Image has a current family that is different from the target queue, a barrier on both queues is needed.
                else
                {
                    barrier.srcQueueFamilyIndex = image->getCurrentFamily()->getIndex();
                    barrier.dstQueueFamilyIndex = familyIndex;

                    releaseBarriers[familyIndex].emplace_back(barrier);
                    acquireBarriers[familyIndex].emplace_back(barrier);
                }
            }
            // If the image has target owner that is not the transfer queue, a barrier on both queues is needed.
            else if (image->getTargetFamily() != transferFamily)
            {
                const auto familyIndex      = image->getTargetFamily()->getIndex();
                barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask       = image->getAccessFlags();
                barrier.oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout           = image->getTargetLayout();
                barrier.srcQueueFamilyIndex = transferFamily->getIndex();
                barrier.dstQueueFamilyIndex = familyIndex;

                releaseBarriers[familyIndex].emplace_back(barrier);
                acquireBarriers[familyIndex].emplace_back(barrier);
            }
            // Target owner is transfer queue, only an acquire barrier on the transfer queue is needed.
            else
            {
                barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask       = image->getAccessFlags();
                barrier.oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout           = image->getTargetLayout();
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                acquireBarriers[transferFamily->getIndex()].emplace_back(barrier);
            }

            // Create union of all stage masks for this queue family.
            acquireDstStageMasks[image->getTargetFamily()->getIndex()] |= image->getStageFlags();

            // Update current owner and layout of image.
            image->setCurrentFamily(*image->getTargetFamily());
            image->setCurrentLayout(image->getTargetLayout());
        }

        // Wait for previous call to this method to complete before resetting and recording command buffers.
        std::vector<VkFence> vkFences;
        for (auto& f : releaseState.fences)
        {
            if (f->isSignaled())
            {
                vkFences.emplace_back(f->get());
                f->setSignaled(false);
            }
        }
        if (!vkFences.empty())
        {
            vkWaitForFences(device.get(), static_cast<uint32_t>(vkFences.size()), vkFences.data(), true, UINT64_MAX);
            vkResetFences(device.get(), static_cast<uint32_t>(vkFences.size()), vkFences.data());
        }

        // Record pipeline barrier commands.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (acquireBarriers[i].empty()) continue;

            if (i != transferFamily->getIndex())
            {
                auto& releaseCmd = *releaseState.releaseCommandBuffers[i];
                releaseCmd.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                releaseCmd.beginOneTimeCommand();

                vkCmdPipelineBarrier(releaseCmd.get(),
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                     0,
                                     0,
                                     nullptr,
                                     0,
                                     nullptr,
                                     static_cast<uint32_t>(releaseBarriers[i].size()),
                                     releaseBarriers[i].data());

                releaseCmd.endCommand();
            }

            {
                auto& acquireCmd = *releaseState.acquireCommandBuffers[i];
                acquireCmd.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                acquireCmd.beginOneTimeCommand();

                vkCmdPipelineBarrier(acquireCmd.get(),
                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                     acquireDstStageMasks[i],  // Union of all stage masks for images on this pipeline.
                                     0,
                                     0,
                                     nullptr,
                                     0,
                                     nullptr,
                                     static_cast<uint32_t>(acquireBarriers[i].size()),
                                     acquireBarriers[i].data());

                acquireCmd.endCommand();
            }
        }

        // Submit pipeline barriers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (acquireBarriers[i].empty()) continue;

            auto& fence = releaseState.fences[i];

            if (i != transferFamily->getIndex())
            {
                // Submits on different queues require a semaphore.
                const auto& semaphore = *releaseState.semaphores[i];

                // Submit release to transfer queue.
                VkSubmitInfo releaseSubmit{};
                releaseSubmit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                releaseSubmit.pNext                = nullptr;
                releaseSubmit.waitSemaphoreCount   = 0;
                releaseSubmit.pWaitSemaphores      = nullptr;
                releaseSubmit.pWaitDstStageMask    = nullptr;
                releaseSubmit.commandBufferCount   = 1;
                releaseSubmit.pCommandBuffers      = &releaseState.releaseCommandBuffers[i]->get();
                releaseSubmit.signalSemaphoreCount = 1;
                releaseSubmit.pSignalSemaphores    = &semaphore.get();
                transferQueue.submit(releaseSubmit);

                // Submit acquire to target queue.
                VkSubmitInfo acquireSubmit{};
                acquireSubmit.sType                   = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                acquireSubmit.pNext                   = nullptr;
                acquireSubmit.waitSemaphoreCount      = 1;
                acquireSubmit.pWaitSemaphores         = &semaphore.get();
                VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                acquireSubmit.pWaitDstStageMask       = &waitDstStageMask;
                acquireSubmit.commandBufferCount      = 1;
                acquireSubmit.pCommandBuffers         = &releaseState.acquireCommandBuffers[i]->get();
                acquireSubmit.signalSemaphoreCount    = 0;
                acquireSubmit.pSignalSemaphores       = nullptr;
                memoryManager->getQueue(i).submit(acquireSubmit, fence->get());
            }
            else
            {
                // Submit acquire to transfer queue.
                VkSubmitInfo acquireSubmit{};
                acquireSubmit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                acquireSubmit.pNext                = nullptr;
                acquireSubmit.waitSemaphoreCount   = 0;
                acquireSubmit.pWaitSemaphores      = nullptr;
                acquireSubmit.pWaitDstStageMask    = nullptr;
                acquireSubmit.commandBufferCount   = 1;
                acquireSubmit.pCommandBuffers      = &releaseState.acquireCommandBuffers[i]->get();
                acquireSubmit.signalSemaphoreCount = 0;
                acquireSubmit.pSignalSemaphores    = nullptr;
                transferQueue.submit(acquireSubmit, fence->get());
            }

            fence->setSignaled(true);
        }
    }

    void DefaultImageTransfer::wait()
    {
        const auto& device = memoryManager->getDevice();

        // Wait for fences that signal completion of the copies.
        if (copyState.fence->isSignaled())
        {
            vkWaitForFences(device.get(), 1, &copyState.fence->get(), true, UINT64_MAX);
            vkResetFences(device.get(), 1, &copyState.fence->get());
            copyState.fence->setSignaled(false);
        }

        // Clear pending copies after fence so that staging buffers can be deallocated.
        for (const auto& buffers : pendingCopies | std::views::values)
            for (const auto& b : buffers) b.stagingBuffer->unmap();
        pendingCopies.clear();
    }

}  // namespace sol
