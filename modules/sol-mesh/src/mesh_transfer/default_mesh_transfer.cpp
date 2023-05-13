#include "sol/mesh/mesh_transfer/default_mesh_transfer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol/core/vulkan_buffer.h"
#include "sol/core/vulkan_command_buffer.h"
#include "sol/core/vulkan_command_pool.h"
#include "sol/core/vulkan_device.h"
#include "sol/core/vulkan_fence.h"
#include "sol/core/vulkan_physical_device.h"
#include "sol/core/vulkan_queue.h"
#include "sol/core/vulkan_queue_family.h"
#include "sol/core/vulkan_semaphore.h"
#include "sol/memory/memory_manager.h"
#include "sol/mesh/i_mesh.h"
#include "sol/mesh/mesh_description.h"
#include "sol/mesh/mesh_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    DefaultMeshTransfer::DefaultMeshTransfer(MeshManager& manager) :
        IMeshTransfer(), memoryManager(&manager.getMemoryManager())
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

    DefaultMeshTransfer::~DefaultMeshTransfer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Transfer.
    ////////////////////////////////////////////////////////////////

    void DefaultMeshTransfer::stageCopy(MeshDescriptionPtr meshDescription, IMesh& mesh)
    {
        stagedCopies.emplace_back(std::move(meshDescription), &mesh);
    }

    void DefaultMeshTransfer::transfer()
    {
        if (stagedCopies.empty()) return;
        // TODO: This class shares a lot of code with the ImageTransfer.
        // This should be generalized, perhaps to some utility stuff in the memory module.
        // While doing that, also look at the todos in ImageTransfer. There are still important missing features, such as ownership transfers, overlapping writes, etc.
        // That way, managers for other types of buffers could also be easily implemented.
        acquireOwnership();
        copyBuffers();
        releaseOwnership();
        pendingCopies = std::move(stagedCopies);
    }

    void DefaultMeshTransfer::acquireOwnership()
    {
        auto&        device         = memoryManager->getDevice();
        auto&        physicalDevice = device.getPhysicalDevice();
        const size_t familyCount    = physicalDevice.getQueueFamilies().size();
        auto&        transferQueue  = memoryManager->getTransferQueue();
        auto*        transferFamily = &transferQueue.getFamily();

        VkBufferMemoryBarrier barrier{};
        barrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.pNext         = nullptr;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.offset        = 0;
        barrier.size          = VK_WHOLE_SIZE;

        std::vector<std::vector<VkBufferMemoryBarrier>> releaseBarriers(familyCount);
        std::vector<std::vector<VkBufferMemoryBarrier>> acquireBarriers(familyCount);
        std::vector<VkBuffer>                           buffers;

        for (const auto& [desc, mesh] : stagedCopies)
        {
            buffers.clear();
            mesh->getVertexBufferHandles(buffers);
            if (mesh->isIndexed()) buffers.emplace_back(mesh->getIndexBufferHandle());

            for (auto b : buffers)
            {
                barrier.buffer = b;

                // If the buffer has an owner that is not the transfer queue, a barrier on both queues is needed.
                if (mesh->getCurrentFamily() && mesh->getCurrentFamily() != transferFamily)
                {
                    barrier.srcQueueFamilyIndex = mesh->getCurrentFamily()->getIndex();
                    barrier.dstQueueFamilyIndex = transferFamily->getIndex();

                    releaseBarriers[mesh->getCurrentFamily()->getIndex()].emplace_back(barrier);
                    acquireBarriers[mesh->getCurrentFamily()->getIndex()].emplace_back(barrier);
                }
                // If the buffer does not yet have an owner or is already owned by the transfer queue, only an acquire barrier on the transfer queue is needed.
                else
                {
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                    acquireBarriers[transferFamily->getIndex()].emplace_back(barrier);
                }
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
                                     static_cast<uint32_t>(releaseBarriers[i].size()),
                                     releaseBarriers[i].data(),
                                     0,
                                     nullptr);

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
                                     static_cast<uint32_t>(acquireBarriers[i].size()),
                                     acquireBarriers[i].data(),
                                     0,
                                     nullptr);

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
                memoryManager->getQueue(i).submit(*acquireState.releaseCommandBuffers[i]);

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

    void DefaultMeshTransfer::copyBuffers()
    {
        const auto& device        = memoryManager->getDevice();
        auto&       transferQueue = memoryManager->getTransferQueue();

        // Wait for previous call to this method to complete before clearing command buffer.
        if (copyState.fence->isSignaled())
        {
            vkWaitForFences(device.get(), 1, &copyState.fence->get(), true, UINT64_MAX);
            vkResetFences(device.get(), 1, &copyState.fence->get());
            copyState.fence->setSignaled(false);
        }

        // Clear pending copies after fence so that staging buffers can be deallocated.
        pendingCopies.clear();

        copyState.commandBuffer->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        copyState.commandBuffer->beginOneTimeCommand();

        std::vector<VkBuffer> vbBindings;

        for (const auto& [desc, mesh] : stagedCopies)
        {
            VkBufferCopy region;
            region.srcOffset = 0;
            region.dstOffset = 0;
            region.size      = 0;

            vbBindings.clear();
            mesh->getVertexBufferHandles(vbBindings);
            for (size_t i = 0; i < desc->getVertexBufferCount(); i++)
            {
                region.dstOffset = desc->getVertexOffset(i) * desc->getVertexSize(i);
                region.size      = desc->getVertexBuffer(i).getSettings().size;
                vkCmdCopyBuffer(
                  copyState.commandBuffer->get(), desc->getVertexBuffer(i).get(), vbBindings[i], 1, &region);
            }

            if (desc->isIndexed())
            {
                region.dstOffset = desc->getIndexOffset() * desc->getIndexSize();
                region.size      = desc->getIndexBuffer().getSettings().size;
                vkCmdCopyBuffer(copyState.commandBuffer->get(),
                                desc->getIndexBuffer().get(),
                                mesh->getIndexBufferHandle(),
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

    void DefaultMeshTransfer::releaseOwnership()
    {
        auto&        device         = memoryManager->getDevice();
        auto&        physicalDevice = device.getPhysicalDevice();
        const size_t familyCount    = physicalDevice.getQueueFamilies().size();
        auto&        transferQueue  = memoryManager->getTransferQueue();
        auto*        transferFamily = &transferQueue.getFamily();

        VkBufferMemoryBarrier barrier;
        barrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.pNext         = nullptr;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.offset        = 0;
        barrier.size          = VK_WHOLE_SIZE;

        std::vector<std::vector<VkBufferMemoryBarrier>> releaseBarriers(familyCount);
        std::vector<std::vector<VkBufferMemoryBarrier>> acquireBarriers(familyCount);
        std::vector<VkPipelineStageFlags>               acquireDstStageMasks(familyCount, 0);
        std::vector<VkBuffer>                           buffers;

        for (const auto& [desc, mesh] : stagedCopies)
        {
            buffers.clear();
            mesh->getVertexBufferHandles(buffers);
            auto accessFlags = mesh->getVertexBufferAccessFlags();
            if (mesh->isIndexed())
            {
                buffers.emplace_back(mesh->getIndexBufferHandle());
                accessFlags.emplace_back(mesh->getIndexBufferAccessFlags());
            }
            assert(buffers.size() == accessFlags.size());

            for (size_t i = 0; i < buffers.size(); i++)
            {
                barrier.buffer = buffers[i];

                // If the buffer has a target owner that is not the transfer queue, a barrier on both queues is needed.
                if (mesh->getTargetFamily() && mesh->getTargetFamily() != transferFamily)
                {
                    barrier.dstAccessMask       = accessFlags[i];
                    barrier.srcQueueFamilyIndex = transferFamily->getIndex();
                    barrier.dstQueueFamilyIndex = mesh->getTargetFamily()->getIndex();

                    releaseBarriers[mesh->getTargetFamily()->getIndex()].emplace_back(barrier);
                    acquireBarriers[mesh->getTargetFamily()->getIndex()].emplace_back(barrier);
                }
                // Only an acquire barrier on the transfer queue is needed.
                else
                {
                    barrier.dstAccessMask       = accessFlags[i];
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                    acquireBarriers[transferFamily->getIndex()].emplace_back(barrier);
                }
            }

            // Create union of all stage masks for this queue family.
            acquireDstStageMasks[mesh->getTargetFamily()->getIndex()] |= mesh->getStageFlags();
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
                                     static_cast<uint32_t>(releaseBarriers[i].size()),
                                     releaseBarriers[i].data(),
                                     0,
                                     nullptr);

                releaseCmd.endCommand();
            }

            {
                auto& acquireCmd = *releaseState.acquireCommandBuffers[i];
                acquireCmd.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                acquireCmd.beginOneTimeCommand();

                vkCmdPipelineBarrier(acquireCmd.get(),
                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                     acquireDstStageMasks[i],  // Union of all stage masks for buffers on this pipeline.
                                     0,
                                     0,
                                     nullptr,
                                     static_cast<uint32_t>(acquireBarriers[i].size()),
                                     acquireBarriers[i].data(),
                                     0,
                                     nullptr);

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
}  // namespace sol
