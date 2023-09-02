#include "sol-memory-test/transfer_manager/multiple_copies.h"

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

void MultipleCopies::operator()()
{
    constexpr uint32_t count0 = 1024;
    constexpr uint32_t count1 = 2048;
    constexpr uint32_t count2 = 16384;
    const auto data0 = std::views::iota(0) | std::views::take(count0) | std::ranges::to<std::vector<uint32_t>>();
    const auto data1 = std::views::iota(1000) | std::views::take(count1) | std::ranges::to<std::vector<uint32_t>>();
    const auto data2 = std::views::iota(10000) | std::views::take(count2) | std::ranges::to<std::vector<uint32_t>>();

    // Allocate a number of buffers.
    sol::IBufferPtr srcBuffer0, dstBuffer0;
    sol::IBufferPtr srcBuffer1, dstBuffer1;
    sol::IBufferPtr srcBuffer2, dstBuffer2;
    expectNoThrow([&] {
        sol::IBufferAllocator::AllocationInfo info{
          .size = sizeof(uint32_t) * count0,
          .bufferUsage =
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
          .alignment       = 0};
        srcBuffer0 = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        dstBuffer0 = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        info.size  = sizeof(uint32_t) * count1;
        srcBuffer1 = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        dstBuffer1 = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        info.size  = sizeof(uint32_t) * count2;
        srcBuffer2 = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        dstBuffer2 = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
    });

    // Transfer data to srcBuffers.
    expectNoThrow([&] {
        const auto                                      transaction = getTransferManager().beginTransaction();
        const sol::BufferTransaction::StagingBufferCopy copy0{
          .dstBuffer = *srcBuffer0, .data = data0.data(), .size = VK_WHOLE_SIZE, .offset = 0};
        const sol::BufferTransaction::StagingBufferCopy copy1{
          .dstBuffer = *srcBuffer1, .data = data1.data(), .size = VK_WHOLE_SIZE, .offset = 0};
        const sol::BufferTransaction::StagingBufferCopy copy2{
          .dstBuffer = *srcBuffer2, .data = data2.data(), .size = VK_WHOLE_SIZE, .offset = 0};
        const sol::BufferTransaction::MemoryBarrier barrier0{.buffer    = *srcBuffer0,
                                                             .dstFamily = nullptr,
                                                             .srcStage  = 0,
                                                             .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                             .srcAccess = 0,
                                                             .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
        const sol::BufferTransaction::MemoryBarrier barrier1{.buffer    = *srcBuffer1,
                                                             .dstFamily = nullptr,
                                                             .srcStage  = 0,
                                                             .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                             .srcAccess = 0,
                                                             .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
        const sol::BufferTransaction::MemoryBarrier barrier2{.buffer    = *srcBuffer2,
                                                             .dstFamily = nullptr,
                                                             .srcStage  = 0,
                                                             .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                             .srcAccess = 0,
                                                             .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
        compareTrue(transaction->stage(copy0, barrier0));
        compareTrue(transaction->stage(copy1, barrier1));
        compareTrue(transaction->stage(copy2, barrier2));
        transaction->commit();
        transaction->wait();
    });


    // Copy from src to dst.
    expectNoThrow([&] {
        const auto                                       transaction = getTransferManager().beginTransaction();
        const sol::BufferTransaction::BufferToBufferCopy copy0{.srcBuffer              = *srcBuffer0,
                                                               .dstBuffer              = *dstBuffer0,
                                                               .size                   = VK_WHOLE_SIZE,
                                                               .srcOffset              = 0,
                                                               .dstOffset              = 0,
                                                               .srcOnDedicatedTransfer = false,
                                                               .dstOnDedicatedTransfer = false};
        const sol::BufferTransaction::BufferToBufferCopy copy1{.srcBuffer              = *srcBuffer1,
                                                               .dstBuffer              = *dstBuffer1,
                                                               .size                   = VK_WHOLE_SIZE,
                                                               .srcOffset              = 0,
                                                               .dstOffset              = 0,
                                                               .srcOnDedicatedTransfer = false,
                                                               .dstOnDedicatedTransfer = false};
        const sol::BufferTransaction::BufferToBufferCopy copy2{.srcBuffer              = *srcBuffer2,
                                                               .dstBuffer              = *dstBuffer2,
                                                               .size                   = VK_WHOLE_SIZE,
                                                               .srcOffset              = 0,
                                                               .dstOffset              = 0,
                                                               .srcOnDedicatedTransfer = false,
                                                               .dstOnDedicatedTransfer = false};
        const sol::BufferTransaction::MemoryBarrier      srcBarrier0{
               .buffer = *srcBuffer0, .dstFamily = nullptr, .srcStage = 0, .dstStage = 0, .srcAccess = 0, .dstAccess = 0};
        const sol::BufferTransaction::MemoryBarrier srcBarrier1{
          .buffer = *srcBuffer1, .dstFamily = nullptr, .srcStage = 0, .dstStage = 0, .srcAccess = 0, .dstAccess = 0};
        const sol::BufferTransaction::MemoryBarrier srcBarrier2{
          .buffer = *srcBuffer2, .dstFamily = nullptr, .srcStage = 0, .dstStage = 0, .srcAccess = 0, .dstAccess = 0};
        const sol::BufferTransaction::MemoryBarrier dstBarrier0{.buffer    = *dstBuffer0,
                                                                .dstFamily = nullptr,
                                                                .srcStage  = 0,
                                                                .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                                .srcAccess = 0,
                                                                .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
        const sol::BufferTransaction::MemoryBarrier dstBarrier1{.buffer    = *dstBuffer1,
                                                                .dstFamily = nullptr,
                                                                .srcStage  = 0,
                                                                .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                                .srcAccess = 0,
                                                                .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
        transaction->stage(copy0, srcBarrier0, dstBarrier0);
        const sol::BufferTransaction::MemoryBarrier dstBarrier2{.buffer    = *dstBuffer2,
                                                                .dstFamily = nullptr,
                                                                .srcStage  = 0,
                                                                .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                                .srcAccess = 0,
                                                                .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
        transaction->stage(copy0, srcBarrier0, dstBarrier0);
        transaction->stage(copy0, srcBarrier0, dstBarrier0);
        transaction->stage(copy1, srcBarrier1, dstBarrier1);
        transaction->stage(copy2, srcBarrier2, dstBarrier2);
        transaction->commit();
        transaction->wait();
    });

    // Compare.
    std::vector<uint32_t> dstData(count0);
    memcpy(dstData.data(), dstBuffer0->getBuffer().getMappedData<uint32_t>(), sizeof(uint32_t) * count0);
    compareEQ(data0, dstData);
    dstData.resize(count1);
    memcpy(dstData.data(), dstBuffer1->getBuffer().getMappedData<uint32_t>(), sizeof(uint32_t) * count1);
    compareEQ(data1, dstData);
    dstData.resize(count2);
    memcpy(dstData.data(), dstBuffer2->getBuffer().getMappedData<uint32_t>(), sizeof(uint32_t) * count2);
    compareEQ(data2, dstData);
}
