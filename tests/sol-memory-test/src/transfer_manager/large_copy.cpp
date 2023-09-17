#include "sol-memory-test/transfer_manager/large_copy.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

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

void LargeCopy::operator()()
{
    // 128 MiB.
    constexpr uint32_t count       = 1024 * 1024 * 32;
    constexpr size_t   bufferCount = 8;
    const auto         data = std::views::iota(0) | std::views::take(count) | std::ranges::to<std::vector<uint32_t>>();

    // Allocate a number of buffers.
    std::vector<sol::IBufferPtr> buffers;
    expectNoThrow([&] {
        constexpr sol::IBufferAllocator::AllocationInfo info{.size        = sizeof(uint32_t) * count,
                                                             .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                             .sharingMode         = VK_SHARING_MODE_EXCLUSIVE,
                                                             .memoryUsage         = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                             .requiredMemoryFlags = 0,
                                                             .preferredMemoryFlags = 0,
                                                             .allocationFlags      = 0,
                                                             .alignment            = 0};
        for (size_t i = 0; i < bufferCount; i++)
            buffers.emplace_back(
              getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Do staging buffer copies. Since the memory pool has less space than we need for all copies,
    // we need to do multiple transactions.
    auto   transaction      = getTransferManager().beginTransaction();
    size_t transactionCount = 0;
    for (size_t i = 0; i < bufferCount; i++)
    {
        const sol::StagingBufferCopy copy{.dstBuffer              = *buffers[i],
                                                             .data                   = data.data(),
                                                             .size                   = sizeof(uint32_t) * count,
                                                             .offset                 = 0,
                                                             .dstOnDedicatedTransfer = true};

        // If memory pool is full, commit transaction and make a new one to redo the current and remaining copies.
        if (!transaction->stage(copy, {}, true))
        {
            transaction->commit();
            transaction = getTransferManager().beginTransaction();
            i--;
            transactionCount++;
        }
    }

    transaction->commit();
    transaction->wait();
    transactionCount++;

    compareEQ(4, transactionCount)
      .info("Expected 8 copies of 128MiB to be done in 4 transactions because staging memory pool is 256MiB.");
}
