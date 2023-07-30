#include "sol-memory-test/free_at_once_memory_pool.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/pool/free_at_once_memory_pool.h"
#include "sol-memory/pool/memory_pool_buffer.h"

void FreeAtOnceMemoryPool::operator()()
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

    // Create a memory pool with 2 blocks of 1MiB.
    sol::FreeAtOnceMemoryPool* pool = nullptr;
    expectNoThrow([&] {
        pool = &memoryManager->createFreeAtOnceMemoryPool(
          "pool", VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, 1024ull * 1024, 0, 2);
    });

    compareEQ(sol::IMemoryPool::Capabilities::None, pool->getCapabilities());

    // Two allocations of half block size.
    std::vector<sol::MemoryPoolBufferPtr> buffers;
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 500)); });
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 500)); });
    compareEQ(1024ull * 500, buffers[0]->getBufferSize());
    compareEQ(1024ull * 500, buffers[1]->getBufferSize());

    // Allocation larger than block size.
    expectThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 2048)); });

    // Fill up remaining blocks.
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 500)); });
    expectNoThrow([&] { buffers.emplace_back(pool->allocateBuffer(1024ull * 500)); });

    // Any new allocation should fail as long as previous allocations were not cleared.
    expectNoThrow([&] { buffers[0].reset(); });
    expectThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 500)); });
    expectNoThrow([&] { buffers[1].reset(); });
    expectThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 500)); });
    expectNoThrow([&] { buffers[2].reset(); });
    expectThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 500)); });
    expectNoThrow([&] { buffers[3].reset(); });
    expectNoThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 500)); });
}
