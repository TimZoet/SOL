#include "sol-memory-test/transfer_manager/partial_copy.h"

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

void PartialCopy::operator()()
{
    constexpr uint32_t elementCount = 1024;
    const auto data = std::views::iota(0) | std::views::take(elementCount) | std::ranges::to<std::vector<uint32_t>>();

    // Create equally sized buffers.
    sol::IBufferPtr srcBuffer, dstBuffer;
    expectNoThrow([&] {
        constexpr sol::IBufferAllocator::AllocationInfo info{
          .size = sizeof(uint32_t) * elementCount,
          .bufferUsage =
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags      = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
          .alignment            = 0};
        srcBuffer = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
        dstBuffer = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
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


    // Transfer first block of 256 elements to first block.
    {
        const auto                                       transaction = getTransferManager().beginTransaction();
        const sol::BufferTransaction::BufferToBufferCopy copy{.srcBuffer              = *srcBuffer,
                                                              .dstBuffer              = *dstBuffer,
                                                              .size                   = sizeof(uint32_t) * 256,
                                                              .srcOffset              = 0,
                                                              .dstOffset              = 0,
                                                              .srcOnDedicatedTransfer = false,
                                                              .dstOnDedicatedTransfer = false};
        const sol::BufferTransaction::MemoryBarrier      srcBarrier{.buffer    = *srcBuffer,
                                                                    .dstFamily = nullptr,
                                                                    .srcStage  = 0,
                                                                    .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                                    .srcAccess = 0,
                                                                    .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
        const sol::BufferTransaction::MemoryBarrier      dstBarrier{
               .buffer    = *dstBuffer,
               .dstFamily = nullptr,
               .srcStage  = 0,
               .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT,
               .srcAccess = 0,
               .dstAccess = VK_ACCESS_2_HOST_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT};
        transaction->stage(copy, srcBarrier, dstBarrier);
        transaction->commit();
        transaction->wait();


        std::vector<uint32_t> srcData(256);
        std::vector<uint32_t> dstData(256);
        memcpy(srcData.data(), data.data(), sizeof(uint32_t) * 256);
        memcpy(dstData.data(), dstBuffer->getBuffer().getMappedData<uint32_t>(), sizeof(uint32_t) * 256);
        compareEQ(srcData, dstData);
    }

    // Transfer fourth block of 256 elements to second block.
    {
        const auto                                       transaction = getTransferManager().beginTransaction();
        const sol::BufferTransaction::BufferToBufferCopy copy{.srcBuffer              = *srcBuffer,
                                                              .dstBuffer              = *dstBuffer,
                                                              .size                   = sizeof(uint32_t) * 256,
                                                              .srcOffset              = sizeof(uint32_t) * 256 * 3,
                                                              .dstOffset              = sizeof(uint32_t) * 256 * 1,
                                                              .srcOnDedicatedTransfer = false,
                                                              .dstOnDedicatedTransfer = false};
        const sol::BufferTransaction::MemoryBarrier      srcBarrier{
               .buffer = *srcBuffer, .dstFamily = nullptr, .srcStage = 0, .dstStage = 0, .srcAccess = 0, .dstAccess = 0};
        const sol::BufferTransaction::MemoryBarrier dstBarrier{.buffer    = *dstBuffer,
                                                               .dstFamily = nullptr,
                                                               .srcStage  = 0,
                                                               .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                               .srcAccess = 0,
                                                               .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
        transaction->stage(copy, srcBarrier, dstBarrier);
        transaction->commit();
        transaction->wait();


        std::vector<uint32_t> srcData(256);
        std::vector<uint32_t> dstData(256);
        std::memcpy(srcData.data(), data.data() + 256ull * 3, sizeof(uint32_t) * 256);
        std::memcpy(dstData.data(), dstBuffer->getBuffer().getMappedData<uint32_t>() + 256, sizeof(uint32_t) * 256);
        compareEQ(srcData, dstData);
    }

    // Transfer second block of 256 elements to second block.
    {
        const auto                                       transaction = getTransferManager().beginTransaction();
        const sol::BufferTransaction::BufferToBufferCopy copy{.srcBuffer              = *srcBuffer,
                                                              .dstBuffer              = *dstBuffer,
                                                              .size                   = sizeof(uint32_t) * 256,
                                                              .srcOffset              = sizeof(uint32_t) * 256,
                                                              .dstOffset              = sizeof(uint32_t) * 256,
                                                              .srcOnDedicatedTransfer = false,
                                                              .dstOnDedicatedTransfer = false};
        const sol::BufferTransaction::MemoryBarrier      srcBarrier{
               .buffer = *srcBuffer, .dstFamily = nullptr, .srcStage = 0, .dstStage = 0, .srcAccess = 0, .dstAccess = 0};
        const sol::BufferTransaction::MemoryBarrier dstBarrier{.buffer    = *dstBuffer,
                                                               .dstFamily = nullptr,
                                                               .srcStage  = 0,
                                                               .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                               .srcAccess = 0,
                                                               .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
        transaction->stage(copy, srcBarrier, dstBarrier);
        transaction->commit();
        transaction->wait();


        std::vector<uint32_t> srcData(256);
        std::vector<uint32_t> dstData(256);
        memcpy(srcData.data(), data.data() + 256, sizeof(uint32_t) * 256);
        memcpy(dstData.data(), dstBuffer->getBuffer().getMappedData<uint32_t>() + 256, sizeof(uint32_t) * 256);
        compareEQ(srcData, dstData);
    }

    // Transfer first and third block of 256 elements to third and fourth block.
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
                                                                  .size                   = sizeof(uint32_t) * 256,
                                                                  .srcOffset              = 0,
                                                                  .dstOffset              = sizeof(uint32_t) * 256 * 2,
                                                                  .srcOnDedicatedTransfer = false,
                                                                  .dstOnDedicatedTransfer = false};

            transaction->stage(copy);
        });

        expectNoThrow([&] {
            const sol::BufferTransaction::BufferToBufferCopy copy{.srcBuffer              = *srcBuffer,
                                                                  .dstBuffer              = *dstBuffer,
                                                                  .size                   = sizeof(uint32_t) * 256,
                                                                  .srcOffset              = sizeof(uint32_t) * 256 * 2,
                                                                  .dstOffset              = sizeof(uint32_t) * 256 * 3,
                                                                  .srcOnDedicatedTransfer = false,
                                                                  .dstOnDedicatedTransfer = false};

            transaction->stage(copy);
        });

        expectNoThrow([&] {
            const sol::BufferTransaction::MemoryBarrier after{.buffer    = *dstBuffer,
                                                              .dstFamily = nullptr,
                                                              .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                              .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                              .srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                                              .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
            transaction->stage(after, sol::BufferTransaction::BarrierLocation::AfterCopy);
        });

        transaction->commit();
        transaction->wait();


        std::vector<uint32_t> srcData(1024);
        std::vector<uint32_t> dstData(1024);
        memcpy(srcData.data() + 256ull * 0, data.data() + 256ull * 0, sizeof(uint32_t) * 256);
        memcpy(srcData.data() + 256ull * 1, data.data() + 256ull * 1, sizeof(uint32_t) * 256);
        memcpy(srcData.data() + 256ull * 2, data.data() + 256ull * 0, sizeof(uint32_t) * 256);
        memcpy(srcData.data() + 256ull * 3, data.data() + 256ull * 2, sizeof(uint32_t) * 256);
        memcpy(dstData.data() + 256ull * 0,
               dstBuffer->getBuffer().getMappedData<uint32_t>() + 256ull * 0,
               sizeof(uint32_t) * 256);
        memcpy(dstData.data() + 256ull * 1,
               dstBuffer->getBuffer().getMappedData<uint32_t>() + 256ull * 1,
               sizeof(uint32_t) * 256);
        memcpy(dstData.data() + 256ull * 2,
               dstBuffer->getBuffer().getMappedData<uint32_t>() + 256ull * 2,
               sizeof(uint32_t) * 256);
        memcpy(dstData.data() + 256ull * 3,
               dstBuffer->getBuffer().getMappedData<uint32_t>() + 256ull * 3,
               sizeof(uint32_t) * 256);
        compareEQ(srcData, dstData);
    }
}