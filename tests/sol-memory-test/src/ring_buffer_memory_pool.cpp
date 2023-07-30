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
#include "sol-memory/memory_pool_buffer.h"
#include "sol-memory/ring_buffer_memory_pool.h"

void RingBufferMemoryPool::operator()()
{
    // NOTE: Always allocating just a little bit below what's indicated. Memory requirements for a buffer
    // tend to be higher since the driver adds some of its own data for bookkeeping.

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
        pool = &memoryManager->createRingBufferMemoryPool(
          "pool", VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, 1024ull * 1024, false);
    });

    compareEQ(sol::IMemoryPool::Capabilities::Wait, pool->getCapabilities());

    // Two allocations of quarter block size.
    std::vector<sol::MemoryPoolBufferPtr> buffers;
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 250)); });
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 250)); });
    compareEQ(1024ull * 250, buffers[0]->getBufferSize());
    compareEQ(1024ull * 250, buffers[1]->getBufferSize());

    // Allocation larger than block size.
    expectThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 2048)); });

    // Fill up remaining blocks.
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 250)); });
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 250)); });

    // Clearing the first buffer should open up space again.
    expectNoThrow([&] { buffers[0].reset(); });
    expectNoThrow([&] { buffers[0] = pool->allocateBuffer(1024ull * 250); });

    // Clearing the third buffer opens up space, but in the wrong place.
    expectNoThrow([&] { buffers[2].reset(); });
    expectThrow([&] { buffers[2] = pool->allocateBuffer(1024ull * 250); });

    // Clearing the second buffer should open up contiguous space at the correct offset again.
    expectNoThrow([&] { buffers[1].reset(); });
    expectNoThrow([&] { buffers[1] = pool->allocateBuffer(1024ull * 250); });
    expectNoThrow([&] { buffers[2] = pool->allocateBuffer(1024ull * 250); });

    // Clear all memory.
    expectNoThrow([&] { buffers.clear(); });

    // Fill up memory.
    expectNoThrow([&] {
        for (size_t i = 0; i < 8; i++) buffers.emplace_back(pool->allocateBuffer(1024ull * 120));
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
                  buffers[index].reset();
                  buffers[index] = pool->allocateBufferWithWait(1024ull * 120);
              },
              7 - i));

        for (auto& f : futures) expectNoThrow([&] { f.get(); }).info("allocateBufferWithWait() threw an exception.");
    }
}
