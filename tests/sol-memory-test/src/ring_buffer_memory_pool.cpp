#include "sol-memory-test/ring_buffer_memory_pool.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <future>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/pool/memory_pool_buffer.h"
#include "sol-memory/pool/ring_buffer_memory_pool.h"

void RingBufferMemoryPool::operator()()
{
    sol::VulkanMemoryAllocator::Settings settings;
    settings.device          = getDevice();
    settings.flags           = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    const auto memoryManager = std::make_unique<sol::MemoryManager>(sol::VulkanMemoryAllocator::create(settings));
    for (auto& queue : getDevice().getQueues())
    {
        if (queue->getFamily().supportsCompute()) memoryManager->setComputeQueue(*queue);
        if (queue->getFamily().supportsGraphics()) memoryManager->setGraphicsQueue(*queue);
        if (queue->getFamily().supportsDedicatedTransfer()) memoryManager->setTransferQueue(*queue);
    }

    // Create a memory pool with 1MiB.
    sol::RingBufferMemoryPool* pool = nullptr;
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::CreateInfo info{.createFlags          = 0,
                                                    .bufferUsage          = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                    .requiredMemoryFlags  = 0,
                                                    .preferredMemoryFlags = 0,
                                                    .allocationFlags      = 0,
                                                    .blockSize            = 1024ull * 1024ull,
                                                    .minBlocks            = 0,
                                                    .maxBlocks            = 1};
        pool = &memoryManager->createRingBufferMemoryPool("pool", info);
    });

    compareEQ(sol::IMemoryPool::Capabilities::Wait, pool->getCapabilities());

    // Two allocations of quarter block size.
    std::vector<sol::MemoryPoolBufferPtr> buffers;
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc));
    });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc));
    });
    compareEQ(1024ull * 256, buffers[0]->getBufferSize());
    compareEQ(1024ull * 256, buffers[1]->getBufferSize());

    // Allocation larger than block size.
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 2048ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc));
    });

    // Fill up remaining blocks.
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc));
    });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc));
    });

    // Clearing the first buffer should open up space again.
    expectNoThrow([&] { buffers[0].reset(); });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers[0] = pool->allocateBuffer(alloc);
    });

    // Clearing the third buffer opens up space, but in the wrong place.
    expectNoThrow([&] { buffers[2].reset(); });
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers[2] = pool->allocateBuffer(alloc);
    });

    // Clearing the second buffer should open up contiguous space at the correct offset again.
    expectNoThrow([&] { buffers[1].reset(); });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers[1] = pool->allocateBuffer(alloc);
    });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        buffers[2] = pool->allocateBuffer(alloc);
    });

    // Clear all memory.
    expectNoThrow([&] { buffers.clear(); });

    // Fill up memory.
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 128ull, .bufferUsage = 0, .alignment = 0};

        for (size_t i = 0; i < 8; i++) buffers.emplace_back(pool->allocateBuffer(alloc));
    });

    // Running a bunch of threads in parallel with std::async. They will release and allocate
    // a buffer effectively in a random order. By enabling waiting, allocation should never throw
    // an out of memory error.
    // Async is non-deterministic. Repeating 1000 times to make test more robust.
    for (size_t j = 0; j < 1000; j++)
    {
        std::vector<std::future<void>> futures;
        for (size_t i = 0; i < 8; i++)
            futures.emplace_back(std::async(
              std::launch::async,
              [&](const size_t index) {
                  constexpr sol::IMemoryPool::AllocationInfo alloc{
                    .size = 1024ull * 128ull, .bufferUsage = 0, .alignment = 0};
                  buffers[index].reset();
                  buffers[index] = pool->allocateBufferWithWait(alloc);
              },
              7 - i));

        for (auto& f : futures) expectNoThrow([&] { f.get(); }).info("allocateBufferWithWait() threw an exception.");
    }
}
