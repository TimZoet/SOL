#include "sol-memory/buffer_transaction.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_timeline_semaphore.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/i_buffer.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transfer_manager.h"

namespace
{
    [[nodiscard]] sol::IBufferPtr tryAllocate(const sol::TransferManager&                      manager,
                                              const sol::BufferTransaction::StagingBufferCopy& copy)
    {
        const sol::IBufferAllocator::AllocationInfo alloc{
          .size                 = copy.size == VK_WHOLE_SIZE ? copy.dstBuffer.getBufferSize() : copy.size,
          .bufferUsage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
          .alignment       = 0};
        auto stagingBuffer =
          manager.getMemoryPool().allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Empty);

        if (!stagingBuffer) return nullptr;

        stagingBuffer->getBuffer().setData(copy.data, stagingBuffer->getBufferSize());
        return stagingBuffer;
    }
}  // namespace

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    BufferTransaction::BufferTransaction(TransferManager& transferManager) : manager(&transferManager) {}

    BufferTransaction::~BufferTransaction() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& BufferTransaction::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& BufferTransaction::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& BufferTransaction::getMemoryManager() noexcept { return manager->getMemoryManager(); }

    const MemoryManager& BufferTransaction::getMemoryManager() const noexcept { return manager->getMemoryManager(); }

    ////////////////////////////////////////////////////////////////
    // Staging.
    ////////////////////////////////////////////////////////////////

    void BufferTransaction::stage(MemoryBarrier barrier, const BarrierLocation location)
    {
        if (location == BarrierLocation::BeforeCopy)
            staged.emplace_back(Element{.preBarrier    = barrier,
                                        .postBarrier   = std::nullopt,
                                        .s2bCopy       = std::nullopt,
                                        .b2bCopy       = std::nullopt,
                                        .stagingBuffer = nullptr});
        else
            staged.emplace_back(Element{.preBarrier    = std::nullopt,
                                        .postBarrier   = barrier,
                                        .s2bCopy       = std::nullopt,
                                        .b2bCopy       = std::nullopt,
                                        .stagingBuffer = nullptr});
    }

    bool BufferTransaction::stage(const StagingBufferCopy& copy, const std::optional<MemoryBarrier>& barrier)
    {
        auto stagingBuffer = tryAllocate(*manager, copy);
        if (!stagingBuffer) return false;

        // Memory barrier that will get the destination buffer from its current state to the transfer state.
        if (barrier)
        {
            stage(MemoryBarrier{.buffer    = copy.dstBuffer,
                                .dstFamily = copy.dstOnDedicatedTransfer ?
                                               &getMemoryManager().getTransferQueue().getFamily() :
                                               nullptr,
                                .srcStage  = barrier->srcStage,
                                .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                .srcAccess = barrier->srcAccess,
                                .dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT},
                  BarrierLocation::BeforeCopy);
        }

        // The actual copy.
        staged.emplace_back(Element{.preBarrier    = std::nullopt,
                                    .postBarrier   = std::nullopt,
                                    .s2bCopy       = copy,
                                    .b2bCopy       = std::nullopt,
                                    .stagingBuffer = std::move(stagingBuffer)});

        // Memory barrier that will get the destination buffer from the transfer state to its final state.
        if (barrier)
        {
            const auto* dstFamily = barrier->dstFamily;
            if (copy.dstOnDedicatedTransfer && !dstFamily) dstFamily = &copy.dstBuffer.getQueueFamily();

            stage(MemoryBarrier{.buffer    = copy.dstBuffer,
                                .dstFamily = dstFamily,
                                .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                .dstStage  = barrier->dstStage,
                                .srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                .dstAccess = barrier->dstStage},
                  BarrierLocation::AfterCopy);
        }

        return true;
    }

    void BufferTransaction::stage(const BufferToBufferCopy&           copy,
                                  const std::optional<MemoryBarrier>& srcBarrier,
                                  const std::optional<MemoryBarrier>& dstBarrier)
    {
        // Memory barrier that will get the source buffer from its current state to the transfer read state.
        if (srcBarrier)
        {
            stage(MemoryBarrier{.buffer    = copy.srcBuffer,
                                .dstFamily = copy.srcOnDedicatedTransfer ?
                                               &getMemoryManager().getTransferQueue().getFamily() :
                                               nullptr,
                                .srcStage  = srcBarrier->srcStage,
                                .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                .srcAccess = srcBarrier->srcAccess,
                                .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT},
                  BarrierLocation::BeforeCopy);
        }

        // Memory barrier that will get the destination buffer from its current state to the transfer write state.
        if (dstBarrier)
        {
            stage(MemoryBarrier{.buffer    = copy.dstBuffer,
                                .dstFamily = copy.dstOnDedicatedTransfer ?
                                               &getMemoryManager().getTransferQueue().getFamily() :
                                               nullptr,
                                .srcStage  = dstBarrier->srcStage,
                                .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                .srcAccess = dstBarrier->srcAccess,
                                .dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT},
                  BarrierLocation::BeforeCopy);
        }

        // The actual copy.
        staged.emplace_back(Element{.preBarrier    = std::nullopt,
                                    .postBarrier   = std::nullopt,
                                    .s2bCopy       = std::nullopt,
                                    .b2bCopy       = copy,
                                    .stagingBuffer = nullptr});

        // Memory barrier that will get the source buffer from the transfer read state to its final state.
        if (srcBarrier)
        {
            const auto* dstFamily = srcBarrier->dstFamily;
            if (copy.dstOnDedicatedTransfer && !dstFamily) dstFamily = &copy.dstBuffer.getQueueFamily();

            stage(MemoryBarrier{.buffer    = copy.srcBuffer,
                                .dstFamily = dstFamily,
                                .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                .dstStage  = srcBarrier->dstStage,
                                .srcAccess = VK_ACCESS_2_TRANSFER_READ_BIT,
                                .dstAccess = srcBarrier->dstStage},
                  BarrierLocation::AfterCopy);
        }

        // Memory barrier that will get the destination buffer from the transfer state to its final state.
        if (dstBarrier)
        {
            const auto* dstFamily = dstBarrier->dstFamily;
            if (copy.dstOnDedicatedTransfer && !dstFamily) dstFamily = &copy.dstBuffer.getQueueFamily();

            stage(MemoryBarrier{.buffer    = copy.dstBuffer,
                                .dstFamily = dstFamily,
                                .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                .dstStage  = dstBarrier->dstStage,
                                .srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                .dstAccess = dstBarrier->dstStage},
                  BarrierLocation::AfterCopy);
        }
    }

    ////////////////////////////////////////////////////////////////
    // Commit.
    ////////////////////////////////////////////////////////////////

    void BufferTransaction::commit()
    {
        if (committed) throw SolError("");

        auto&      memoryManager  = getMemoryManager();
        auto&      device         = memoryManager.getDevice();
        auto&      physicalDevice = device.getPhysicalDevice();
        const auto familyCount    = static_cast<uint32_t>(physicalDevice.getQueueFamilies().size());

        // Barriers releasing ownership from current queue, submitted before copies.
        std::vector<std::vector<VkBufferMemoryBarrier2>> preCopyReleaseBarriers(familyCount);
        // Barriers acquiring ownership on destination queue, submitted before copies.
        // Queue can also be the transfer queue as an intermediate before the final
        // release-acquire is done post copy.
        std::vector<std::vector<VkBufferMemoryBarrier2>> preCopyAcquireBarriers(familyCount);
        // Barriers releasing ownership, submitted after copies.
        // Queue can also be the transfer queue that was used as an intermediate.
        std::vector<std::vector<VkBufferMemoryBarrier2>> postCopyReleaseBarriers(familyCount);
        // Barriers acquiring ownership on destination queue, submitted after copies.
        std::vector<std::vector<VkBufferMemoryBarrier2>> postCopyAcquireBarriers(familyCount);

        std::vector<VkBufferCopy2>     bufferCopies;
        std::vector<VkCopyBufferInfo2> copies;

        // Collect all barriers and copies.
        for (const auto& [preBarrier, postBarrier, s2bCopy, b2bCopy, stagingBuffer] : staged)
        {
            // Barrier before copy.
            if (preBarrier)
            {
                const auto& srcFamily = preBarrier->buffer.getQueueFamily();
                const auto& dstFamily = preBarrier->dstFamily ? *preBarrier->dstFamily : srcFamily;

                // Source and destionation family are the same. Only an acquire on the destination queue is needed.
                if (&srcFamily == &dstFamily)
                {
                    preCopyAcquireBarriers[dstFamily.getIndex()].emplace_back(
                      VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                             .pNext               = nullptr,
                                             .srcStageMask        = preBarrier->srcStage,
                                             .srcAccessMask       = preBarrier->srcAccess,
                                             .dstStageMask        = preBarrier->dstStage,
                                             .dstAccessMask       = preBarrier->dstAccess,
                                             .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                             .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                             .buffer              = preBarrier->buffer.getBuffer().get(),
                                             .offset              = preBarrier->buffer.getBufferOffset(),
                                             .size                = preBarrier->buffer.getBufferSize()});
                }
                // Source and destination family are different. Release and acquire are needed.
                else
                {
                    preCopyReleaseBarriers[srcFamily.getIndex()].emplace_back(
                      VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                             .pNext               = nullptr,
                                             .srcStageMask        = preBarrier->srcStage,
                                             .srcAccessMask       = preBarrier->srcAccess,
                                             .dstStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                             .dstAccessMask       = VK_ACCESS_2_NONE,
                                             .srcQueueFamilyIndex = srcFamily.getIndex(),
                                             .dstQueueFamilyIndex = dstFamily.getIndex(),
                                             .buffer              = preBarrier->buffer.getBuffer().get(),
                                             .offset              = preBarrier->buffer.getBufferOffset(),
                                             .size                = preBarrier->buffer.getBufferSize()});

                    preCopyAcquireBarriers[dstFamily.getIndex()].emplace_back(
                      VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                             .pNext               = nullptr,
                                             .srcStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                             .srcAccessMask       = VK_ACCESS_2_NONE,
                                             .dstStageMask        = preBarrier->dstStage,
                                             .dstAccessMask       = preBarrier->dstAccess,
                                             .srcQueueFamilyIndex = srcFamily.getIndex(),
                                             .dstQueueFamilyIndex = dstFamily.getIndex(),
                                             .buffer              = preBarrier->buffer.getBuffer().get(),
                                             .offset              = preBarrier->buffer.getBufferOffset(),
                                             .size                = preBarrier->buffer.getBufferSize()});

                    // Update queue family.
                    preBarrier->buffer.setQueueFamily(dstFamily);
                }
            }
            //  Barrier after copy.
            else if (postBarrier)
            {
                const auto& srcFamily = postBarrier->buffer.getQueueFamily();
                const auto& dstFamily = postBarrier->dstFamily ? *postBarrier->dstFamily : srcFamily;

                // Source and destionation family are the same. Only an acquire on the destination queue is needed.
                if (&srcFamily == &dstFamily)
                {
                    postCopyAcquireBarriers[dstFamily.getIndex()].emplace_back(
                      VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                             .pNext               = nullptr,
                                             .srcStageMask        = postBarrier->srcStage,
                                             .srcAccessMask       = postBarrier->srcAccess,
                                             .dstStageMask        = postBarrier->dstStage,
                                             .dstAccessMask       = postBarrier->dstAccess,
                                             .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                             .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                             .buffer              = postBarrier->buffer.getBuffer().get(),
                                             .offset              = postBarrier->buffer.getBufferOffset(),
                                             .size                = postBarrier->buffer.getBufferSize()});
                }
                // Source and destination family are different. Release and acquire are needed.
                else
                {
                    postCopyReleaseBarriers[srcFamily.getIndex()].emplace_back(
                      VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                             .pNext               = nullptr,
                                             .srcStageMask        = postBarrier->srcStage,
                                             .srcAccessMask       = postBarrier->srcAccess,
                                             .dstStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                             .dstAccessMask       = VK_ACCESS_2_NONE,
                                             .srcQueueFamilyIndex = srcFamily.getIndex(),
                                             .dstQueueFamilyIndex = dstFamily.getIndex(),
                                             .buffer              = postBarrier->buffer.getBuffer().get(),
                                             .offset              = postBarrier->buffer.getBufferOffset(),
                                             .size                = postBarrier->buffer.getBufferSize()});

                    postCopyAcquireBarriers[dstFamily.getIndex()].emplace_back(
                      VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                             .pNext               = nullptr,
                                             .srcStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                             .srcAccessMask       = VK_ACCESS_2_NONE,
                                             .dstStageMask        = postBarrier->dstStage,
                                             .dstAccessMask       = postBarrier->dstAccess,
                                             .srcQueueFamilyIndex = srcFamily.getIndex(),
                                             .dstQueueFamilyIndex = dstFamily.getIndex(),
                                             .buffer              = postBarrier->buffer.getBuffer().get(),
                                             .offset              = postBarrier->buffer.getBufferOffset(),
                                             .size                = postBarrier->buffer.getBufferSize()});

                    // Update queue family.
                    postBarrier->buffer.setQueueFamily(dstFamily);
                }
            }
            // Staging buffer copy.
            else if (s2bCopy)
            {
                bufferCopies.emplace_back(VkBufferCopy2{
                  .sType     = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                  .pNext     = nullptr,
                  .srcOffset = stagingBuffer->getBufferOffset(),
                  .dstOffset = s2bCopy->offset + s2bCopy->dstBuffer.getBufferOffset(),
                  .size      = s2bCopy->size == VK_WHOLE_SIZE ? s2bCopy->dstBuffer.getBufferSize() : s2bCopy->size});
            }
            // Buffer to buffer copy.
            else if (b2bCopy)
            {
                bufferCopies.emplace_back(VkBufferCopy2{
                  .sType     = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                  .pNext     = nullptr,
                  .srcOffset = b2bCopy->srcOffset + b2bCopy->srcBuffer.getBufferOffset(),
                  .dstOffset = b2bCopy->dstOffset + b2bCopy->dstBuffer.getBufferOffset(),
                  .size      = b2bCopy->size == VK_WHOLE_SIZE ? b2bCopy->srcBuffer.getBufferSize() : b2bCopy->size});
            }
            else
                assert(false);
        }

        // Collect copy infos. (Needs to happen after bufferCopies was fully filled for stable pointers.)
        size_t copyIndex = 0;
        for (const auto& [preBarrier, postBarrier, s2bCopy, b2bCopy, stagingBuffer] : staged)
        {
            if (s2bCopy)
            {
                copies.emplace_back(VkCopyBufferInfo2{.sType       = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                                      .pNext       = nullptr,
                                                      .srcBuffer   = stagingBuffer->getBuffer().get(),
                                                      .dstBuffer   = s2bCopy->dstBuffer.getBuffer().get(),
                                                      .regionCount = 1,
                                                      .pRegions    = &bufferCopies[copyIndex++]});
            }
            // Copy.
            else if (b2bCopy)
            {
                copies.emplace_back(VkCopyBufferInfo2{.sType       = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                                      .pNext       = nullptr,
                                                      .srcBuffer   = b2bCopy->srcBuffer.getBuffer().get(),
                                                      .dstBuffer   = b2bCopy->dstBuffer.getBuffer().get(),
                                                      .regionCount = 1,
                                                      .pRegions    = &bufferCopies[copyIndex++]});
            }
        }

        auto lock = manager->lock();
        manager->wait();
        index = ++manager->transactionIndex;

        // Submit pre-copy release barriers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (preCopyReleaseBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->preCopyReleaseCmdBuffers[i];

            const VkDependencyInfo dependency{.sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                              .pNext              = VK_NULL_HANDLE,
                                              .dependencyFlags    = 0,
                                              .memoryBarrierCount = 0,
                                              .pMemoryBarriers    = VK_NULL_HANDLE,
                                              .bufferMemoryBarrierCount =
                                                static_cast<uint32_t>(preCopyReleaseBarriers[i].size()),
                                              .pBufferMemoryBarriers   = preCopyReleaseBarriers[i].data(),
                                              .imageMemoryBarrierCount = 0,
                                              .pImageMemoryBarriers    = VK_NULL_HANDLE};

            cmdBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            cmdBuffer.beginOneTimeCommand();
            vkCmdPipelineBarrier2(cmdBuffer.get(), &dependency);
            cmdBuffer.endCommand();

            const VkSemaphoreSubmitInfo signalSemaphore{.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                                        .pNext       = VK_NULL_HANDLE,
                                                        .semaphore   = manager->semaphores[i]->get(),
                                                        .value       = ++manager->semaphoreValues[i],
                                                        .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
                                                        .deviceIndex = 0};

            const VkCommandBufferSubmitInfo commandSubmit{.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                                          .pNext         = nullptr,
                                                          .commandBuffer = cmdBuffer.get(),
                                                          .deviceMask    = 0};

            const VkSubmitInfo2 submit{.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                                       .pNext                    = nullptr,
                                       .flags                    = 0,
                                       .waitSemaphoreInfoCount   = 0,
                                       .pWaitSemaphoreInfos      = VK_NULL_HANDLE,
                                       .commandBufferInfoCount   = 1,
                                       .pCommandBufferInfos      = &commandSubmit,
                                       .signalSemaphoreInfoCount = 1,
                                       .pSignalSemaphoreInfos    = &signalSemaphore};

            handleVulkanError(vkQueueSubmit2(memoryManager.getQueue(i).get(), 1, &submit, VK_NULL_HANDLE));
        }

        // Submit pre-copy acquire barriers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (preCopyAcquireBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->preCopyAcquireCmdBuffers[i];

            const VkDependencyInfo dependency{.sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                              .pNext              = VK_NULL_HANDLE,
                                              .dependencyFlags    = 0,
                                              .memoryBarrierCount = 0,
                                              .pMemoryBarriers    = VK_NULL_HANDLE,
                                              .bufferMemoryBarrierCount =
                                                static_cast<uint32_t>(preCopyAcquireBarriers[i].size()),
                                              .pBufferMemoryBarriers   = preCopyAcquireBarriers[i].data(),
                                              .imageMemoryBarrierCount = 0,
                                              .pImageMemoryBarriers    = VK_NULL_HANDLE};

            cmdBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            cmdBuffer.beginOneTimeCommand();
            vkCmdPipelineBarrier2(cmdBuffer.get(), &dependency);
            cmdBuffer.endCommand();

            std::vector<VkSemaphoreSubmitInfo> waitSemaphores;
            for (uint32_t j = 0; j < familyCount; j++)
            {
                // Wait on the same queue is not needed.
                if (i == j) continue;

                waitSemaphores.emplace_back(
                  VkSemaphoreSubmitInfo{.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                        .pNext       = VK_NULL_HANDLE,
                                        .semaphore   = manager->semaphores[j]->get(),
                                        .value       = manager->semaphoreValues[j],
                                        .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
                                        .deviceIndex = 0});
            }

            const VkSemaphoreSubmitInfo signalSemaphore{.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                                        .pNext       = VK_NULL_HANDLE,
                                                        .semaphore   = manager->semaphores[i]->get(),
                                                        .value       = ++manager->semaphoreValues[i],
                                                        .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
                                                        .deviceIndex = 0};

            const VkCommandBufferSubmitInfo commandSubmit{.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                                          .pNext         = nullptr,
                                                          .commandBuffer = cmdBuffer.get(),
                                                          .deviceMask    = 0};

            const VkSubmitInfo2 submit{.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                                       .pNext                    = nullptr,
                                       .flags                    = 0,
                                       .waitSemaphoreInfoCount   = static_cast<uint32_t>(waitSemaphores.size()),
                                       .pWaitSemaphoreInfos      = waitSemaphores.data(),
                                       .commandBufferInfoCount   = 1,
                                       .pCommandBufferInfos      = &commandSubmit,
                                       .signalSemaphoreInfoCount = 1,
                                       .pSignalSemaphoreInfos    = &signalSemaphore};

            handleVulkanError(vkQueueSubmit2(memoryManager.getQueue(i).get(), 1, &submit, VK_NULL_HANDLE));
        }

        // Submit copies.
        if (!copies.empty())
        {
            auto& transferQueue = getMemoryManager().getTransferQueue();
            auto& cmdBuffer     = *manager->copyCmdBuffer;

            cmdBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            cmdBuffer.beginOneTimeCommand();
            for (const auto& cp : copies) vkCmdCopyBuffer2(cmdBuffer.get(), &cp);
            cmdBuffer.endCommand();

            std::vector<VkSemaphoreSubmitInfo> waitSemaphores;
            for (uint32_t j = 0; j < familyCount; j++)
            {
                // Wait on the same queue is not needed.
                if (transferQueue.getFamily().getIndex() == j) continue;

                waitSemaphores.emplace_back(
                  VkSemaphoreSubmitInfo{.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                        .pNext       = VK_NULL_HANDLE,
                                        .semaphore   = manager->semaphores[j]->get(),
                                        .value       = manager->semaphoreValues[j],
                                        .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
                                        .deviceIndex = 0});
            }

            const VkSemaphoreSubmitInfo signalSemaphore{
              .sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
              .pNext       = VK_NULL_HANDLE,
              .semaphore   = manager->semaphores[transferQueue.getFamily().getIndex()]->get(),
              .value       = ++manager->semaphoreValues[transferQueue.getFamily().getIndex()],
              .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
              .deviceIndex = 0};

            const VkCommandBufferSubmitInfo commandSubmit{.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                                          .pNext         = nullptr,
                                                          .commandBuffer = cmdBuffer.get(),
                                                          .deviceMask    = 0};

            const VkSubmitInfo2 submit{.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                                       .pNext                    = nullptr,
                                       .flags                    = 0,
                                       .waitSemaphoreInfoCount   = static_cast<uint32_t>(waitSemaphores.size()),
                                       .pWaitSemaphoreInfos      = waitSemaphores.data(),
                                       .commandBufferInfoCount   = 1,
                                       .pCommandBufferInfos      = &commandSubmit,
                                       .signalSemaphoreInfoCount = 1,
                                       .pSignalSemaphoreInfos    = &signalSemaphore};

            handleVulkanError(vkQueueSubmit2(transferQueue.get(), 1, &submit, VK_NULL_HANDLE));
        }

        // Submit post-copy release barriers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (postCopyReleaseBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->postCopyReleaseCmdBuffers[i];

            const VkDependencyInfo dependency{.sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                              .pNext              = VK_NULL_HANDLE,
                                              .dependencyFlags    = 0,
                                              .memoryBarrierCount = 0,
                                              .pMemoryBarriers    = VK_NULL_HANDLE,
                                              .bufferMemoryBarrierCount =
                                                static_cast<uint32_t>(postCopyReleaseBarriers[i].size()),
                                              .pBufferMemoryBarriers   = postCopyReleaseBarriers[i].data(),
                                              .imageMemoryBarrierCount = 0,
                                              .pImageMemoryBarriers    = VK_NULL_HANDLE};

            cmdBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            cmdBuffer.beginOneTimeCommand();
            vkCmdPipelineBarrier2(cmdBuffer.get(), &dependency);
            cmdBuffer.endCommand();

            const VkSemaphoreSubmitInfo signalSemaphore{.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                                        .pNext       = VK_NULL_HANDLE,
                                                        .semaphore   = manager->semaphores[i]->get(),
                                                        .value       = ++manager->semaphoreValues[i],
                                                        .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
                                                        .deviceIndex = 0};

            const VkCommandBufferSubmitInfo commandSubmit{.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                                          .pNext         = nullptr,
                                                          .commandBuffer = cmdBuffer.get(),
                                                          .deviceMask    = 0};

            const VkSubmitInfo2 submit{.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                                       .pNext                    = nullptr,
                                       .flags                    = 0,
                                       .waitSemaphoreInfoCount   = 0,
                                       .pWaitSemaphoreInfos      = VK_NULL_HANDLE,
                                       .commandBufferInfoCount   = 1,
                                       .pCommandBufferInfos      = &commandSubmit,
                                       .signalSemaphoreInfoCount = 1,
                                       .pSignalSemaphoreInfos    = &signalSemaphore};

            handleVulkanError(vkQueueSubmit2(memoryManager.getQueue(i).get(), 1, &submit, VK_NULL_HANDLE));
        }

        // Submit post-copy acquire barriers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (postCopyAcquireBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->postCopyAcquireCmdBuffers[i];

            const VkDependencyInfo dependency{.sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                              .pNext              = VK_NULL_HANDLE,
                                              .dependencyFlags    = 0,
                                              .memoryBarrierCount = 0,
                                              .pMemoryBarriers    = VK_NULL_HANDLE,
                                              .bufferMemoryBarrierCount =
                                                static_cast<uint32_t>(postCopyAcquireBarriers[i].size()),
                                              .pBufferMemoryBarriers   = postCopyAcquireBarriers[i].data(),
                                              .imageMemoryBarrierCount = 0,
                                              .pImageMemoryBarriers    = VK_NULL_HANDLE};

            cmdBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            cmdBuffer.beginOneTimeCommand();
            vkCmdPipelineBarrier2(cmdBuffer.get(), &dependency);
            cmdBuffer.endCommand();

            std::vector<VkSemaphoreSubmitInfo> waitSemaphores;
            for (uint32_t j = 0; j < familyCount; j++)
            {
                // Wait on the same queue is not needed.
                if (i == j) continue;

                waitSemaphores.emplace_back(
                  VkSemaphoreSubmitInfo{.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                        .pNext       = VK_NULL_HANDLE,
                                        .semaphore   = manager->semaphores[j]->get(),
                                        .value       = manager->semaphoreValues[j],
                                        .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
                                        .deviceIndex = 0});
            }

            const VkSemaphoreSubmitInfo signalSemaphore{.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
                                                        .pNext       = VK_NULL_HANDLE,
                                                        .semaphore   = manager->semaphores[i]->get(),
                                                        .value       = ++manager->semaphoreValues[i],
                                                        .stageMask   = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,  //TODO
                                                        .deviceIndex = 0};

            const VkCommandBufferSubmitInfo commandSubmit{.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
                                                          .pNext         = nullptr,
                                                          .commandBuffer = cmdBuffer.get(),
                                                          .deviceMask    = 0};

            const VkSubmitInfo2 submit{.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
                                       .pNext                    = nullptr,
                                       .flags                    = 0,
                                       .waitSemaphoreInfoCount   = static_cast<uint32_t>(waitSemaphores.size()),
                                       .pWaitSemaphoreInfos      = waitSemaphores.data(),
                                       .commandBufferInfoCount   = 1,
                                       .pCommandBufferInfos      = &commandSubmit,
                                       .signalSemaphoreInfoCount = 1,
                                       .pSignalSemaphoreInfos    = &signalSemaphore};

            handleVulkanError(vkQueueSubmit2(memoryManager.getQueue(i).get(), 1, &submit, VK_NULL_HANDLE));
        }


        committed = true;
    }

    void BufferTransaction::wait()
    {
        if (done) return;
        if (!committed) throw SolError("Cannot wait an a BufferTransaction that was not yet committed.");

        auto lock = manager->lock();
        if (manager->transactionIndex == index) manager->wait();

        done = true;
    }

}  // namespace sol
