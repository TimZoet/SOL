#include "sol-memory/buffer_transaction.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_timeline_semaphore.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/i_buffer.h"
#include "sol-memory/i_image.h"
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

    [[nodiscard]] sol::IBufferPtr tryAllocate(const sol::TransferManager&                     manager,
                                              const sol::BufferTransaction::StagingImageCopy& copy)
    {
        const sol::IBufferAllocator::AllocationInfo alloc{
          .size                 = copy.dataSize,
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

    BufferTransaction::~BufferTransaction() noexcept
    {
        /*
         * If necessary, collect pending staging buffers. They will be destroyed in the next wait of the manager.
         */

        if (!committed || (s2bCopies.empty() && s2iCopies.empty())) return;

        std::vector<IBufferPtr> stagingBuffers;
        for (auto& buffer : s2bCopies | std::views::values) stagingBuffers.push_back(std::move(buffer));
        for (auto& buffer : s2iCopies | std::views::values) stagingBuffers.push_back(std::move(buffer));

        auto lock = manager->lock();
        manager->pendingStagingBuffers.reserve(manager->pendingStagingBuffers.size() + stagingBuffers.size());
        for (auto& b : stagingBuffers) manager->pendingStagingBuffers.push_back(std::move(b));
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& BufferTransaction::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& BufferTransaction::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& BufferTransaction::getMemoryManager() noexcept { return manager->getMemoryManager(); }

    const MemoryManager& BufferTransaction::getMemoryManager() const noexcept { return manager->getMemoryManager(); }

    TransferManager& BufferTransaction::getTransferManager() noexcept { return *manager; }

    const TransferManager& BufferTransaction::getTransferManager() const noexcept { return *manager; }

    const std::vector<uint64_t>& BufferTransaction::getSemaphoreValues() const
    {
        if (!committed) throw SolError("Cannot get semaphore values of BufferTransaction before it is committed.");
        return semaphoreValues;
    }

    ////////////////////////////////////////////////////////////////
    // Staging.
    ////////////////////////////////////////////////////////////////

    void BufferTransaction::stage(MemoryBarrier barrier, const BarrierLocation location)
    {
        // TODO: All stage methods should check transaction was not yet committed.

        if (location == BarrierLocation::BeforeCopy)
            preBufferBarriers.emplace_back(barrier);
        else
            postBufferBarriers.emplace_back(barrier);
    }

    void BufferTransaction::stage(ImageBarrier barrier, BarrierLocation location)
    {
        if (location == BarrierLocation::BeforeCopy)
            preImageBarriers.emplace_back(barrier);
        else
            postImageBarriers.emplace_back(barrier);
    }

    bool BufferTransaction::stage(const StagingBufferCopy&            copy,
                                  const std::optional<MemoryBarrier>& barrier,
                                  const bool                          waitOnAllocFailure)
    {
        // TODO: Here and in the other stage method we could test for image/bufferUsage being transfer_dst/src.

        auto stagingBuffer = tryAllocate(*manager, copy);
        if (!stagingBuffer)
        {
            if (waitOnAllocFailure)
            {
                auto lock     = manager->lockAndWait();
                stagingBuffer = tryAllocate(*manager, copy);
                if (!stagingBuffer) return false;
            }
            else
                return false;
        }

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
        s2bCopies.emplace_back(copy, std::move(stagingBuffer));

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

    bool BufferTransaction::stage(const StagingImageCopy&            copy,
                                  const std::optional<ImageBarrier>& barrier,
                                  const bool                         waitOnAllocFailure)
    {
        // TODO: If there is a barrier, it is currently assumed that the levels and layers it describes match the regions in the copy.

        auto stagingBuffer = tryAllocate(*manager, copy);
        if (!stagingBuffer)
        {
            if (waitOnAllocFailure)
            {
                auto lock     = manager->lockAndWait();
                stagingBuffer = tryAllocate(*manager, copy);
                if (!stagingBuffer) return false;
            }
            else
                return false;
        }

        // Image barrier that will get the destination image from its current state to the transfer state.
        if (barrier)
        {
            stage(ImageBarrier{.image          = copy.dstImage,
                               .dstFamily      = copy.dstOnDedicatedTransfer ?
                                                   &getMemoryManager().getTransferQueue().getFamily() :
                                                   nullptr,
                               .srcStage       = barrier->srcStage,
                               .dstStage       = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                               .srcAccess      = barrier->srcAccess,
                               .dstAccess      = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                               .srcLayout      = barrier->srcLayout,
                               .dstLayout      = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               .aspectMask     = barrier->aspectMask,
                               .baseMipLevel   = barrier->baseMipLevel,
                               .levelCount     = barrier->levelCount,
                               .baseArrayLayer = barrier->baseArrayLayer,
                               .layerCount     = barrier->layerCount},
                  BarrierLocation::BeforeCopy);
        }

        // The actual copy.
        s2iCopies.emplace_back(copy, std::move(stagingBuffer));

        // Memory barrier that will get the destination image from the transfer state to its final state.
        if (barrier)
        {
            const auto* dstFamily = barrier->dstFamily;
            // TODO: This assumes that all levels and layers are owned by the same queue (also done during the commit).
            // If they have different owners, you must stage barriers/copies separately. Make a note of that in the docs.
            if (copy.dstOnDedicatedTransfer && !dstFamily)
                dstFamily = &copy.dstImage.getQueueFamily(barrier->baseMipLevel, barrier->baseArrayLayer);

            stage(ImageBarrier{.image          = copy.dstImage,
                               .dstFamily      = dstFamily,
                               .srcStage       = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                               .dstStage       = barrier->dstStage,
                               .srcAccess      = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                               .dstAccess      = barrier->dstStage,
                               .srcLayout      = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               .dstLayout      = barrier->dstLayout,
                               .aspectMask     = barrier->aspectMask,
                               .baseMipLevel   = barrier->baseMipLevel,
                               .levelCount     = barrier->levelCount,
                               .baseArrayLayer = barrier->baseArrayLayer,
                               .layerCount     = barrier->layerCount},
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
        b2bCopies.emplace_back(copy);

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

    void BufferTransaction::stage(const ImageToBufferCopy&            copy,
                                  const std::optional<ImageBarrier>&  srcBarrier,
                                  const std::optional<MemoryBarrier>& dstBarrier)
    {
        // TODO: If there is a barrier, it is currently assumed that the levels and layers it describes match the regions in the copy.
        // Image barrier that will get the source image from its current state to the transfer read state.
        if (srcBarrier)
        {
            stage(ImageBarrier{.image          = copy.srcImage,
                               .dstFamily      = copy.dstOnDedicatedTransfer ?
                                                   &getMemoryManager().getTransferQueue().getFamily() :
                                                   nullptr,
                               .srcStage       = srcBarrier->srcStage,
                               .dstStage       = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                               .srcAccess      = srcBarrier->srcAccess,
                               .dstAccess      = VK_ACCESS_2_TRANSFER_READ_BIT,
                               .srcLayout      = srcBarrier->srcLayout,
                               .dstLayout      = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               .aspectMask     = srcBarrier->aspectMask,
                               .baseMipLevel   = srcBarrier->baseMipLevel,
                               .levelCount     = srcBarrier->levelCount,
                               .baseArrayLayer = srcBarrier->baseArrayLayer,
                               .layerCount     = srcBarrier->layerCount},
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
        i2bCopies.emplace_back(copy);

        // Image barrier that will get the source image from the transfer read state to its final state.
        if (srcBarrier)
        {
            stage(ImageBarrier{.image          = copy.srcImage,
                               .dstFamily      = srcBarrier->dstFamily,
                               .srcStage       = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                               .dstStage       = srcBarrier->dstStage,
                               .srcAccess      = VK_ACCESS_2_TRANSFER_READ_BIT,
                               .dstAccess      = srcBarrier->dstAccess,
                               .srcLayout      = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               .dstLayout      = srcBarrier->dstLayout,
                               .aspectMask     = srcBarrier->aspectMask,
                               .baseMipLevel   = srcBarrier->baseMipLevel,
                               .levelCount     = srcBarrier->levelCount,
                               .baseArrayLayer = srcBarrier->baseArrayLayer,
                               .layerCount     = srcBarrier->layerCount},
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
        std::vector<std::vector<VkBufferMemoryBarrier2>> preCopyReleaseBufferBarriers(familyCount);
        std::vector<std::vector<VkImageMemoryBarrier2>>  preCopyReleaseImageBarriers(familyCount);
        // Barriers acquiring ownership on destination queue, submitted before copies.
        // Queue can also be the transfer queue as an intermediate before the final
        // release-acquire is done post copy.
        std::vector<std::vector<VkBufferMemoryBarrier2>> preCopyAcquireBufferBarriers(familyCount);
        std::vector<std::vector<VkImageMemoryBarrier2>>  preCopyAcquireImageBarriers(familyCount);
        // Barriers releasing ownership, submitted after copies.
        // Queue can also be the transfer queue that was used as an intermediate.
        std::vector<std::vector<VkBufferMemoryBarrier2>> postCopyReleaseBufferBarriers(familyCount);
        std::vector<std::vector<VkImageMemoryBarrier2>>  postCopyReleaseImageBarriers(familyCount);
        // Barriers acquiring ownership on destination queue, submitted after copies.
        std::vector<std::vector<VkBufferMemoryBarrier2>> postCopyAcquireBufferBarriers(familyCount);
        std::vector<std::vector<VkImageMemoryBarrier2>>  postCopyAcquireImageBarriers(familyCount);

        std::vector<VkBufferCopy2>            bufferCopies;  // [s2b[0], ..., s2b[n], b2b[0], ..., b2b[n]]
        std::vector<VkImageCopy2>             imageCopies;
        std::vector<VkBufferImageCopy2>       bufferImageCopies;  // [s2i[0], ..., s2i[n], b2i[0], ..., b2i[n]]
        std::vector<VkBufferImageCopy2>       imageBufferCopies;
        std::vector<VkCopyBufferInfo2>        bufferInfos;
        std::vector<VkCopyImageInfo2>         imageInfos;
        std::vector<VkCopyBufferToImageInfo2> bufferImageInfos;
        std::vector<VkCopyImageToBufferInfo2> imageBufferInfos;

        for (const auto& barrier : preBufferBarriers)
        {
            const auto& srcFamily = barrier.buffer.getQueueFamily();
            const auto& dstFamily = barrier.dstFamily ? *barrier.dstFamily : srcFamily;

            // Source and destionation family are the same. Only an acquire on the destination queue is needed.
            if (&srcFamily == &dstFamily)
            {
                preCopyAcquireBufferBarriers[dstFamily.getIndex()].emplace_back(
                  VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                         .pNext               = nullptr,
                                         .srcStageMask        = barrier.srcStage,
                                         .srcAccessMask       = barrier.srcAccess,
                                         .dstStageMask        = barrier.dstStage,
                                         .dstAccessMask       = barrier.dstAccess,
                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .buffer              = barrier.buffer.getBuffer().get(),
                                         .offset              = barrier.buffer.getBufferOffset(),
                                         .size                = barrier.buffer.getBufferSize()});
            }
            // Source and destination family are different. Release and acquire are needed.
            else
            {
                preCopyReleaseBufferBarriers[srcFamily.getIndex()].emplace_back(
                  VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                         .pNext               = nullptr,
                                         .srcStageMask        = barrier.srcStage,
                                         .srcAccessMask       = barrier.srcAccess,
                                         .dstStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                         .dstAccessMask       = VK_ACCESS_2_NONE,
                                         .srcQueueFamilyIndex = srcFamily.getIndex(),
                                         .dstQueueFamilyIndex = dstFamily.getIndex(),
                                         .buffer              = barrier.buffer.getBuffer().get(),
                                         .offset              = barrier.buffer.getBufferOffset(),
                                         .size                = barrier.buffer.getBufferSize()});

                preCopyAcquireBufferBarriers[dstFamily.getIndex()].emplace_back(
                  VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                         .pNext               = nullptr,
                                         .srcStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                         .srcAccessMask       = VK_ACCESS_2_NONE,
                                         .dstStageMask        = barrier.dstStage,
                                         .dstAccessMask       = barrier.dstAccess,
                                         .srcQueueFamilyIndex = srcFamily.getIndex(),
                                         .dstQueueFamilyIndex = dstFamily.getIndex(),
                                         .buffer              = barrier.buffer.getBuffer().get(),
                                         .offset              = barrier.buffer.getBufferOffset(),
                                         .size                = barrier.buffer.getBufferSize()});

                // Update queue family.
                barrier.buffer.setQueueFamily(dstFamily);
            }
        }

        for (const auto& barrier : postBufferBarriers)
        {
            const auto& srcFamily = barrier.buffer.getQueueFamily();
            const auto& dstFamily = barrier.dstFamily ? *barrier.dstFamily : srcFamily;

            // Source and destionation family are the same. Only an acquire on the destination queue is needed.
            if (&srcFamily == &dstFamily)
            {
                postCopyAcquireBufferBarriers[dstFamily.getIndex()].emplace_back(
                  VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                         .pNext               = nullptr,
                                         .srcStageMask        = barrier.srcStage,
                                         .srcAccessMask       = barrier.srcAccess,
                                         .dstStageMask        = barrier.dstStage,
                                         .dstAccessMask       = barrier.dstAccess,
                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .buffer              = barrier.buffer.getBuffer().get(),
                                         .offset              = barrier.buffer.getBufferOffset(),
                                         .size                = barrier.buffer.getBufferSize()});
            }
            // Source and destination family are different. Release and acquire are needed.
            else
            {
                postCopyReleaseBufferBarriers[srcFamily.getIndex()].emplace_back(
                  VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                         .pNext               = nullptr,
                                         .srcStageMask        = barrier.srcStage,
                                         .srcAccessMask       = barrier.srcAccess,
                                         .dstStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                         .dstAccessMask       = VK_ACCESS_2_NONE,
                                         .srcQueueFamilyIndex = srcFamily.getIndex(),
                                         .dstQueueFamilyIndex = dstFamily.getIndex(),
                                         .buffer              = barrier.buffer.getBuffer().get(),
                                         .offset              = barrier.buffer.getBufferOffset(),
                                         .size                = barrier.buffer.getBufferSize()});

                postCopyAcquireBufferBarriers[dstFamily.getIndex()].emplace_back(
                  VkBufferMemoryBarrier2{.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                                         .pNext               = nullptr,
                                         .srcStageMask        = VK_PIPELINE_STAGE_2_NONE,
                                         .srcAccessMask       = VK_ACCESS_2_NONE,
                                         .dstStageMask        = barrier.dstStage,
                                         .dstAccessMask       = barrier.dstAccess,
                                         .srcQueueFamilyIndex = srcFamily.getIndex(),
                                         .dstQueueFamilyIndex = dstFamily.getIndex(),
                                         .buffer              = barrier.buffer.getBuffer().get(),
                                         .offset              = barrier.buffer.getBufferOffset(),
                                         .size                = barrier.buffer.getBufferSize()});

                // Update queue family.
                barrier.buffer.setQueueFamily(dstFamily);
            }
        }

        for (const auto& barrier : preImageBarriers)
        {
            const auto& srcFamily = barrier.image.getQueueFamily(barrier.baseMipLevel, barrier.baseArrayLayer);
            const auto& dstFamily = barrier.dstFamily ? *barrier.dstFamily : srcFamily;

            // Source and destionation family are the same. Only an acquire on the destination queue is needed.
            if (&srcFamily == &dstFamily)
            {
                preCopyAcquireImageBarriers[dstFamily.getIndex()].emplace_back(VkImageMemoryBarrier2{
                  .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .pNext               = nullptr,
                  .srcStageMask        = barrier.srcStage,
                  .srcAccessMask       = barrier.srcAccess,
                  .dstStageMask        = barrier.dstStage,
                  .dstAccessMask       = barrier.dstAccess,
                  .oldLayout           = barrier.srcLayout,
                  .newLayout           = barrier.dstLayout,
                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                  .image               = barrier.image.getImage().get(),
                  .subresourceRange    = VkImageSubresourceRange{.aspectMask     = barrier.aspectMask,
                                                                 .baseMipLevel   = barrier.baseMipLevel,
                                                                 .levelCount     = barrier.levelCount,
                                                                 .baseArrayLayer = barrier.baseArrayLayer,
                                                                 .layerCount     = barrier.layerCount}});
            }
            // Source and destination family are different. Release and acquire are needed.
            else
            {
                preCopyReleaseImageBarriers[srcFamily.getIndex()].emplace_back(VkImageMemoryBarrier2{
                  .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .pNext               = nullptr,
                  .srcStageMask        = barrier.srcStage,
                  .srcAccessMask       = barrier.srcAccess,
                  .dstStageMask        = VK_PIPELINE_STAGE_2_NONE,
                  .dstAccessMask       = VK_ACCESS_2_NONE,
                  .oldLayout           = barrier.srcLayout,
                  .newLayout           = barrier.dstLayout,
                  .srcQueueFamilyIndex = srcFamily.getIndex(),
                  .dstQueueFamilyIndex = dstFamily.getIndex(),
                  .image               = barrier.image.getImage().get(),
                  .subresourceRange    = VkImageSubresourceRange{.aspectMask     = barrier.aspectMask,
                                                                 .baseMipLevel   = barrier.baseMipLevel,
                                                                 .levelCount     = barrier.levelCount,
                                                                 .baseArrayLayer = barrier.baseArrayLayer,
                                                                 .layerCount     = barrier.layerCount}});

                preCopyAcquireImageBarriers[dstFamily.getIndex()].emplace_back(VkImageMemoryBarrier2{
                  .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .pNext               = nullptr,
                  .srcStageMask        = VK_PIPELINE_STAGE_2_NONE,
                  .srcAccessMask       = VK_ACCESS_2_NONE,
                  .dstStageMask        = barrier.dstStage,
                  .dstAccessMask       = barrier.dstAccess,
                  .oldLayout           = barrier.srcLayout,
                  .newLayout           = barrier.dstLayout,
                  .srcQueueFamilyIndex = srcFamily.getIndex(),
                  .dstQueueFamilyIndex = dstFamily.getIndex(),
                  .image               = barrier.image.getImage().get(),
                  .subresourceRange    = VkImageSubresourceRange{.aspectMask     = barrier.aspectMask,
                                                                 .baseMipLevel   = barrier.baseMipLevel,
                                                                 .levelCount     = barrier.levelCount,
                                                                 .baseArrayLayer = barrier.baseArrayLayer,
                                                                 .layerCount     = barrier.layerCount}});

                // Update queue family.
                for (uint32_t level = barrier.baseMipLevel; level < barrier.baseMipLevel + barrier.levelCount; level++)
                    for (uint32_t layer = barrier.baseArrayLayer; layer < barrier.baseArrayLayer + barrier.layerCount;
                         layer++)
                        barrier.image.setQueueFamily(dstFamily, level, layer);
            }
        }

        for (const auto& barrier : postImageBarriers)
        {
            const auto& srcFamily = barrier.image.getQueueFamily(barrier.baseMipLevel, barrier.baseArrayLayer);
            const auto& dstFamily = barrier.dstFamily ? *barrier.dstFamily : srcFamily;

            // Source and destionation family are the same. Only an acquire on the destination queue is needed.
            if (&srcFamily == &dstFamily)
            {
                postCopyAcquireImageBarriers[dstFamily.getIndex()].emplace_back(VkImageMemoryBarrier2{
                  .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .pNext               = nullptr,
                  .srcStageMask        = barrier.srcStage,
                  .srcAccessMask       = barrier.srcAccess,
                  .dstStageMask        = barrier.dstStage,
                  .dstAccessMask       = barrier.dstAccess,
                  .oldLayout           = barrier.srcLayout,
                  .newLayout           = barrier.dstLayout,
                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                  .image               = barrier.image.getImage().get(),
                  .subresourceRange    = VkImageSubresourceRange{.aspectMask     = barrier.aspectMask,
                                                                 .baseMipLevel   = barrier.baseMipLevel,
                                                                 .levelCount     = barrier.levelCount,
                                                                 .baseArrayLayer = barrier.baseArrayLayer,
                                                                 .layerCount     = barrier.layerCount}});
            }
            // Source and destination family are different. Release and acquire are needed.
            else
            {
                postCopyReleaseImageBarriers[srcFamily.getIndex()].emplace_back(VkImageMemoryBarrier2{
                  .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .pNext               = nullptr,
                  .srcStageMask        = barrier.srcStage,
                  .srcAccessMask       = barrier.srcAccess,
                  .dstStageMask        = VK_PIPELINE_STAGE_2_NONE,
                  .dstAccessMask       = VK_ACCESS_2_NONE,
                  .oldLayout           = barrier.srcLayout,
                  .newLayout           = barrier.dstLayout,
                  .srcQueueFamilyIndex = srcFamily.getIndex(),
                  .dstQueueFamilyIndex = dstFamily.getIndex(),
                  .image               = barrier.image.getImage().get(),
                  .subresourceRange    = VkImageSubresourceRange{.aspectMask     = barrier.aspectMask,
                                                                 .baseMipLevel   = barrier.baseMipLevel,
                                                                 .levelCount     = barrier.levelCount,
                                                                 .baseArrayLayer = barrier.baseArrayLayer,
                                                                 .layerCount     = barrier.layerCount}});

                postCopyAcquireImageBarriers[dstFamily.getIndex()].emplace_back(VkImageMemoryBarrier2{
                  .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                  .pNext               = nullptr,
                  .srcStageMask        = VK_PIPELINE_STAGE_2_NONE,
                  .srcAccessMask       = VK_ACCESS_2_NONE,
                  .dstStageMask        = barrier.dstStage,
                  .dstAccessMask       = barrier.dstAccess,
                  .oldLayout           = barrier.srcLayout,
                  .newLayout           = barrier.dstLayout,
                  .srcQueueFamilyIndex = srcFamily.getIndex(),
                  .dstQueueFamilyIndex = dstFamily.getIndex(),
                  .image               = barrier.image.getImage().get(),
                  .subresourceRange    = VkImageSubresourceRange{.aspectMask     = barrier.aspectMask,
                                                                 .baseMipLevel   = barrier.baseMipLevel,
                                                                 .levelCount     = barrier.levelCount,
                                                                 .baseArrayLayer = barrier.baseArrayLayer,
                                                                 .layerCount     = barrier.layerCount}});

                // Update queue family.
                for (uint32_t level = barrier.baseMipLevel; level < barrier.baseMipLevel + barrier.levelCount; level++)
                    for (uint32_t layer = barrier.baseArrayLayer; layer < barrier.baseArrayLayer + barrier.layerCount;
                         layer++)
                        barrier.image.setQueueFamily(dstFamily, level, layer);
            }
        }

        // Collect copies from staging buffers to buffers.
        for (const auto& [copy, buffer] : s2bCopies)
        {
            bufferCopies.emplace_back(
              VkBufferCopy2{.sType     = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                            .pNext     = nullptr,
                            .srcOffset = buffer->getBufferOffset(),
                            .dstOffset = copy.offset + copy.dstBuffer.getBufferOffset(),
                            .size      = copy.size == VK_WHOLE_SIZE ? copy.dstBuffer.getBufferSize() : copy.size});
        }

        // Collect copies from staging buffers to images.
        for (const auto& [copy, buffer] : s2iCopies)
        {
            for (const auto& region : copy.regions)
                bufferImageCopies.emplace_back(VkBufferImageCopy2{
                  .sType             = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
                  .pNext             = nullptr,
                  .bufferOffset      = buffer->getBufferOffset() + region.dataOffset,
                  .bufferRowLength   = 0,
                  .bufferImageHeight = 0,
                  .imageSubresource  = VkImageSubresourceLayers{.aspectMask     = region.aspectMask,
                                                                .mipLevel       = region.mipLevel,
                                                                .baseArrayLayer = region.baseArrayLayer,
                                                                .layerCount     = region.layerCount},
                  .imageOffset       = VkOffset3D{region.offset[0], region.offset[1], region.offset[2]},
                  .imageExtent       = VkExtent3D{region.extent[0], region.extent[1], region.extent[2]}});
        }

        // Collect copies from buffers to buffers.
        for (const auto& copy : b2bCopies)
        {
            bufferCopies.emplace_back(
              VkBufferCopy2{.sType     = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                            .pNext     = nullptr,
                            .srcOffset = copy.srcOffset + copy.srcBuffer.getBufferOffset(),
                            .dstOffset = copy.dstOffset + copy.dstBuffer.getBufferOffset(),
                            .size      = copy.size == VK_WHOLE_SIZE ? copy.srcBuffer.getBufferSize() : copy.size});
        }

        // Collect copies from images to images.
        for (const auto& copy : i2iCopies)
        {
            // TODO:
            static_cast<void>(copy);
        }

        // Collect copies from buffers to images.
        for (const auto& copy : b2iCopies)
        {
            // TODO:
            static_cast<void>(copy);
        }

        // Collect copies from images to buffers.
        for (const auto& [srcImage, dstBuffer, regions, srcOnDedicatedTransfer, dstOnDedicatedTransfer] : i2bCopies)
        {
            for (const auto& region : regions)
                imageBufferCopies.emplace_back(VkBufferImageCopy2{
                  .sType             = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
                  .pNext             = nullptr,
                  .bufferOffset      = dstBuffer.getBufferOffset() + region.dataOffset,
                  .bufferRowLength   = 0,
                  .bufferImageHeight = 0,
                  .imageSubresource  = VkImageSubresourceLayers{.aspectMask     = region.aspectMask,
                                                                .mipLevel       = region.mipLevel,
                                                                .baseArrayLayer = region.baseArrayLayer,
                                                                .layerCount     = region.layerCount},
                  .imageOffset       = VkOffset3D{region.offset[0], region.offset[1], region.offset[2]},
                  .imageExtent       = VkExtent3D{region.extent[0], region.extent[1], region.extent[2]}});
        }

        /*
         * Collect copy infos. Needs to happen after copies were fully collected for stable pointers.
         */

        size_t copyIndex = 0;

        // Collect copy infos from staging buffers to buffers.
        for (const auto& [copy, buffer] : s2bCopies)
        {
            bufferInfos.emplace_back(VkCopyBufferInfo2{.sType       = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                                       .pNext       = nullptr,
                                                       .srcBuffer   = buffer->getBuffer().get(),
                                                       .dstBuffer   = copy.dstBuffer.getBuffer().get(),
                                                       .regionCount = 1,
                                                       .pRegions    = &bufferCopies[copyIndex++]});
        }
        // Collect copy infos from buffers to buffers.
        for (const auto& [srcBuffer,
                          dstBuffer,
                          size,
                          srcOffset,
                          dstOffset,
                          srcOnDedicatedTransfer,
                          dstOnDedicatedTransfer] : b2bCopies)
        {
            bufferInfos.emplace_back(VkCopyBufferInfo2{.sType       = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                                       .pNext       = nullptr,
                                                       .srcBuffer   = srcBuffer.getBuffer().get(),
                                                       .dstBuffer   = dstBuffer.getBuffer().get(),
                                                       .regionCount = 1,
                                                       .pRegions    = &bufferCopies[copyIndex++]});
        }

        copyIndex = 0;

        // Collect copy infos from staging buffers to images.
        for (const auto& [copy, buffer] : s2iCopies)
        {
            bufferImageInfos.emplace_back(
              VkCopyBufferToImageInfo2{.sType          = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
                                       .pNext          = nullptr,
                                       .srcBuffer      = buffer->getBuffer().get(),
                                       .dstImage       = copy.dstImage.getImage().get(),
                                       .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       .regionCount    = static_cast<uint32_t>(copy.regions.size()),
                                       .pRegions       = &bufferImageCopies[copyIndex]});
            copyIndex += copy.regions.size();
        }
        // Collect copy infos from buffers to images
        for (const auto& copy : b2iCopies)
        {
            // TODO:
            // bufferImageInfos.emplace_back
            static_cast<void>(copy);
        }

        copyIndex = 0;

        // Collect copy infos from images to images
        for (const auto& copy : i2iCopies)
        {
            // TODO:
            // imageInfos.emplace_back()
            static_cast<void>(copy);
        }

        copyIndex = 0;

        // Collect copy infos from images to buffers.
        for (const auto& [srcImage, dstBuffer, regions, srcOnDedicatedTransfer, dstOnDedicatedTransfer] : i2bCopies)
        {
            imageBufferInfos.emplace_back(
              VkCopyImageToBufferInfo2{.sType          = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2,
                                       .pNext          = nullptr,
                                       .srcImage       = srcImage.getImage().get(),
                                       .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                       .dstBuffer      = dstBuffer.getBuffer().get(),
                                       .regionCount    = static_cast<uint32_t>(regions.size()),
                                       .pRegions       = &imageBufferCopies[copyIndex]});
            copyIndex += regions.size();
        }

        // Lock manager and wait on previous commits.
        auto lock = manager->lockAndWait();
        index     = ++manager->transactionIndex;

        // Submit pre-copy release barriers.
        for (uint32_t i = 0; i < familyCount; i++)
        {
            if (preCopyReleaseBufferBarriers[i].empty() && preCopyReleaseImageBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->preCopyReleaseCmdBuffers[i];

            const VkDependencyInfo dependency{
              .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
              .pNext                    = VK_NULL_HANDLE,
              .dependencyFlags          = 0,
              .memoryBarrierCount       = 0,
              .pMemoryBarriers          = VK_NULL_HANDLE,
              .bufferMemoryBarrierCount = static_cast<uint32_t>(preCopyReleaseBufferBarriers[i].size()),
              .pBufferMemoryBarriers    = preCopyReleaseBufferBarriers[i].data(),
              .imageMemoryBarrierCount  = static_cast<uint32_t>(preCopyReleaseImageBarriers[i].size()),
              .pImageMemoryBarriers     = preCopyReleaseImageBarriers[i].data()};

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
            if (preCopyAcquireBufferBarriers[i].empty() && preCopyAcquireImageBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->preCopyAcquireCmdBuffers[i];

            const VkDependencyInfo dependency{
              .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
              .pNext                    = VK_NULL_HANDLE,
              .dependencyFlags          = 0,
              .memoryBarrierCount       = 0,
              .pMemoryBarriers          = VK_NULL_HANDLE,
              .bufferMemoryBarrierCount = static_cast<uint32_t>(preCopyAcquireBufferBarriers[i].size()),
              .pBufferMemoryBarriers    = preCopyAcquireBufferBarriers[i].data(),
              .imageMemoryBarrierCount  = static_cast<uint32_t>(preCopyAcquireImageBarriers[i].size()),
              .pImageMemoryBarriers     = preCopyAcquireImageBarriers[i].data()};

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
        if (!bufferInfos.empty() || !imageInfos.empty() || !bufferImageInfos.empty() || !imageBufferInfos.empty())
        {
            auto& transferQueue = getMemoryManager().getTransferQueue();
            auto& cmdBuffer     = *manager->copyCmdBuffer;

            cmdBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            cmdBuffer.beginOneTimeCommand();
            for (const auto& cp : bufferInfos) vkCmdCopyBuffer2(cmdBuffer.get(), &cp);
            for (const auto& cp : imageInfos) vkCmdCopyImage2(cmdBuffer.get(), &cp);
            for (const auto& cp : bufferImageInfos) vkCmdCopyBufferToImage2(cmdBuffer.get(), &cp);
            for (const auto& cp : imageBufferInfos) vkCmdCopyImageToBuffer2(cmdBuffer.get(), &cp);
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
            if (postCopyReleaseBufferBarriers[i].empty() && postCopyReleaseImageBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->postCopyReleaseCmdBuffers[i];

            const VkDependencyInfo dependency{
              .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
              .pNext                    = VK_NULL_HANDLE,
              .dependencyFlags          = 0,
              .memoryBarrierCount       = 0,
              .pMemoryBarriers          = VK_NULL_HANDLE,
              .bufferMemoryBarrierCount = static_cast<uint32_t>(postCopyReleaseBufferBarriers[i].size()),
              .pBufferMemoryBarriers    = postCopyReleaseBufferBarriers[i].data(),
              .imageMemoryBarrierCount  = static_cast<uint32_t>(postCopyReleaseImageBarriers[i].size()),
              .pImageMemoryBarriers     = postCopyReleaseImageBarriers[i].data()};

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
            if (postCopyAcquireBufferBarriers[i].empty() && postCopyAcquireImageBarriers[i].empty()) continue;

            auto& cmdBuffer = *manager->postCopyAcquireCmdBuffers[i];

            const VkDependencyInfo dependency{
              .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
              .pNext                    = VK_NULL_HANDLE,
              .dependencyFlags          = 0,
              .memoryBarrierCount       = 0,
              .pMemoryBarriers          = VK_NULL_HANDLE,
              .bufferMemoryBarrierCount = static_cast<uint32_t>(postCopyAcquireBufferBarriers[i].size()),
              .pBufferMemoryBarriers    = postCopyAcquireBufferBarriers[i].data(),
              .imageMemoryBarrierCount  = static_cast<uint32_t>(postCopyAcquireImageBarriers[i].size()),
              .pImageMemoryBarriers     = postCopyAcquireImageBarriers[i].data()};

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

        // Copy final state of semaphore values.
        semaphoreValues = manager->semaphoreValues;

        committed = true;
    }

    void BufferTransaction::wait()
    {
        if (done) return;
        if (!committed) throw SolError("Cannot wait on a BufferTransaction that was not yet committed.");

        auto lock = manager->lock();

        // We only need to wait if this is still the active transaction.
        // Otherwise, another transaction was already submitted, which had to perform the wait.
        if (manager->transactionIndex == index) manager->wait();

        // Clear out all staging buffers.
        s2bCopies.clear();
        s2iCopies.clear();

        done = true;
    }

}  // namespace sol
