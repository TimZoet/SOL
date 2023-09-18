#include "sol-memory-test/transfer_manager/concurrent_buffer_transactions.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <atomic>
#include <future>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_queue.h"
#include "sol-memory/i_buffer.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction.h"
#include "sol-memory/transaction_manager.h"

void ConcurrentBufferTransactions::operator()()
{
    constexpr uint32_t elementCount = 256;
    const auto data = std::views::iota(0) | std::views::take(elementCount) | std::ranges::to<std::vector<uint32_t>>();

    std::atomic_bool success = true;

    // Running a bunch of threads in parallel with std::async. They will perform a transaction effectively in a random order.
    // Async is non-deterministic. Repeating 100 times to make test more robust.
    for (size_t j = 0; j < 100; j++)
    {
        std::vector<std::future<void>> futures;
        for (size_t i = 0; i < 16; i++)
            futures.emplace_back(std::async(std::launch::async, [&] {
                // Create equally sized buffers, one on the device and the other on the host.
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
                const auto                            deviceBuffer =
                  getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
                info.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
                info.allocationFlags =
                  VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                const auto hostBuffer =
                  getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);

                // Transfer data to device buffer.
                {
                    const auto                   transaction = getTransferManager().beginTransaction();
                    const sol::StagingBufferCopy copy{
                      .dstBuffer = *deviceBuffer, .data = data.data(), .size = VK_WHOLE_SIZE, .offset = 0};
                    const sol::BufferBarrier barrier{.buffer    = *deviceBuffer,
                                                     .dstFamily = nullptr,
                                                     .srcStage  = 0,
                                                     .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                     .srcAccess = 0,
                                                     .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT};
                    if (!transaction->stage(copy, barrier))
                        throw std::runtime_error("Creation of staging buffer failed.");
                    transaction->commit();
                    transaction->wait();
                }

                // Transfer data from device buffer to host buffer.
                {
                    const auto                    transaction = getTransferManager().beginTransaction();
                    const sol::BufferToBufferCopy copy{.srcBuffer              = *deviceBuffer,
                                                       .dstBuffer              = *hostBuffer,
                                                       .size                   = VK_WHOLE_SIZE,
                                                       .srcOffset              = 0,
                                                       .dstOffset              = 0,
                                                       .srcOnDedicatedTransfer = false,
                                                       .dstOnDedicatedTransfer = false};
                    const sol::BufferBarrier      srcBarrier{.buffer    = *deviceBuffer,
                                                             .dstFamily = nullptr,
                                                             .srcStage  = 0,
                                                             .dstStage  = 0,
                                                             .srcAccess = 0,
                                                             .dstAccess = 0};
                    const sol::BufferBarrier      dstBarrier{.buffer    = *hostBuffer,
                                                             .dstFamily = nullptr,
                                                             .srcStage  = 0,
                                                             .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                             .srcAccess = 0,
                                                             .dstAccess = VK_ACCESS_2_HOST_READ_BIT};
                    transaction->stage(copy, srcBarrier, dstBarrier);
                    transaction->commit();
                    transaction->wait();
                }

                // Verify contents of host buffer.
                std::vector<uint32_t> hostData(elementCount);
                memcpy(
                  hostData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), sizeof(uint32_t) * elementCount);
                if (std::memcmp(hostData.data(), data.data(), sizeof(uint32_t) * elementCount) != 0) success = false;
            }));

        for (auto& f : futures)
            expectNoThrow([&] { f.get(); }).info("One of the async buffer transactions threw an exception.");
    }

    compareTrue(success).info("One of the async buffer transactions copied back an invalid host buffer.");
}
