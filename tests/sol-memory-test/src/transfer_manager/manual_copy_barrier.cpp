#include "sol-memory-test/transfer_manager/manual_copy_barrier.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_queue.h"
#include "sol-memory/buffer_transaction.h"
#include "sol-memory/i_buffer.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transfer_manager.h"

void ManualCopyBarrier::operator()()
{
    constexpr uint32_t elementCount = 1024;
    const auto data = std::views::iota(0) | std::views::take(elementCount) | std::ranges::to<std::vector<uint32_t>>();

    // Create equally sized buffers.
    sol::IBufferPtr srcBuffer, dstBuffer, hostBuffer;
    expectNoThrow([&] {
        sol::IBufferAllocator::AllocationInfo info{.size        = sizeof(uint32_t) * elementCount,
                                                   .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                   .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
                                                   .memoryUsage          = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                   .requiredMemoryFlags  = 0,
                                                   .preferredMemoryFlags = 0,
                                                   .allocationFlags      = 0,
                                                   .alignment            = 0};
        srcBuffer        = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        dstBuffer        = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        info.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        info.allocationFlags =
          VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        hostBuffer = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
    });

    // Transfer data to srcBuffer.
    expectNoThrow([&] {
        const auto                                      transaction = getTransferManager().beginTransaction();
        const sol::BufferTransaction::StagingBufferCopy copy{
          .dstBuffer = *srcBuffer, .data = data.data(), .size = VK_WHOLE_SIZE, .offset = 0};
        const sol::BufferTransaction::MemoryBarrier barrier{.buffer    = *srcBuffer,
                                                            .dstFamily = nullptr,
                                                            .srcStage  = 0,
                                                            .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                            .srcAccess = 0,
                                                            .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
        compareTrue(transaction->stage(copy, barrier));
        transaction->commit();
        transaction->wait();
    });

    // Transfer data from srcBuffer to dstBuffer with manual barriers.
    {
        const auto transaction = getTransferManager().beginTransaction();

        expectNoThrow([&] {
            const sol::BufferTransaction::MemoryBarrier before{.buffer    = *srcBuffer,
                                                               .dstFamily = nullptr,
                                                               .srcStage  = 0,
                                                               .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                               .srcAccess = 0,
                                                               .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
            transaction->stage(before, sol::BufferTransaction::BarrierLocation::BeforeCopy);
        });

        expectNoThrow([&] {
            const sol::BufferTransaction::MemoryBarrier before{.buffer    = *dstBuffer,
                                                               .dstFamily = nullptr,
                                                               .srcStage  = 0,
                                                               .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                               .srcAccess = 0,
                                                               .dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT};
            transaction->stage(before, sol::BufferTransaction::BarrierLocation::BeforeCopy);
        });

        expectNoThrow([&] {
            const sol::BufferTransaction::BufferToBufferCopy copy{.srcBuffer              = *srcBuffer,
                                                                  .dstBuffer              = *dstBuffer,
                                                                  .size                   = VK_WHOLE_SIZE,
                                                                  .srcOffset              = 0,
                                                                  .dstOffset              = 0,
                                                                  .srcOnDedicatedTransfer = false,
                                                                  .dstOnDedicatedTransfer = false};
            transaction->stage(copy);
        });

        expectNoThrow([&] {
            const sol::BufferTransaction::MemoryBarrier before{.buffer    = *srcBuffer,
                                                               .dstFamily = nullptr,
                                                               .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                               .dstStage  = 0,
                                                               .srcAccess = VK_ACCESS_2_TRANSFER_READ_BIT,
                                                               .dstAccess = 0};
            transaction->stage(before, sol::BufferTransaction::BarrierLocation::AfterCopy);
        });

        expectNoThrow([&] {
            const sol::BufferTransaction::MemoryBarrier before{.buffer    = *dstBuffer,
                                                               .dstFamily = nullptr,
                                                               .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                               .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                               .srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                                               .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
            transaction->stage(before, sol::BufferTransaction::BarrierLocation::AfterCopy);
        });

        transaction->commit();
        transaction->wait();
    }

    // Transfer data from dstBuffer to host buffer.
    {
        const auto                                       transaction = getTransferManager().beginTransaction();
        const sol::BufferTransaction::BufferToBufferCopy copy{.srcBuffer              = *dstBuffer,
                                                              .dstBuffer              = *hostBuffer,
                                                              .size                   = VK_WHOLE_SIZE,
                                                              .srcOffset              = 0,
                                                              .dstOffset              = 0,
                                                              .srcOnDedicatedTransfer = false,
                                                              .dstOnDedicatedTransfer = false};
        const sol::BufferTransaction::MemoryBarrier      srcBarrier{
               .buffer = *dstBuffer, .dstFamily = nullptr, .srcStage = 0, .dstStage = 0, .srcAccess = 0, .dstAccess = 0};
        const sol::BufferTransaction::MemoryBarrier dstBarrier{.buffer    = *hostBuffer,
                                                               .dstFamily = nullptr,
                                                               .srcStage  = 0,
                                                               .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                               .srcAccess = 0,
                                                               .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
        expectNoThrow([&] { transaction->stage(copy, srcBarrier, dstBarrier); });
        transaction->commit();
        transaction->wait();
    }

    // Verify contents of host buffer.
    std::vector<uint32_t> hostData(elementCount);
    memcpy(hostData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), sizeof(uint32_t) * elementCount);
    compareEQ(data, hostData);
}
