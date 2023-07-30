#include "sol-memory-test/non_linear_memory_pool.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/pool/memory_pool_buffer.h"
#include "sol-memory/pool/non_linear_memory_pool.h"

void NonLinearMemoryPool::operator()()
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

    // Create a memory pool with 10 blocks of 1MiB.
    sol::NonLinearMemoryPool* pool = nullptr;
    expectNoThrow([&] {
        pool = &memoryManager->createNonLinearMemoryPool(
          "pool", VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, 1024ull * 1024, 0, 10);
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
    expectNoThrow([&] {
        for (size_t i = 0; i < 9; i++) buffers.emplace_back(pool->allocateBuffer(1024ull * 1000));
    });

    // Out of memory.
    expectThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 128)); });

    // This pool type does not support waiting.
    expectThrow([&] { static_cast<void>(pool->allocateBufferWithWait(1024ull * 500)); });

    // Clear up some space.
    expectNoThrow([&] { buffers[0].reset(); });

    // Still out of memory because buffer[0] used only half a block.
    expectThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 1000)); });

    // But this should work again.
    expectNoThrow([&] { static_cast<void>(pool->allocateBuffer(1024ull * 500)); });

    // Clear out everything and then fill the whole thing again with a bunch of small allocations.
    expectNoThrow([&] { buffers.clear(); });
    expectNoThrow([&] {
        for (size_t i = 0; i < 320; i++) buffers.emplace_back(pool->allocateBuffer(1024ull * 30));
    });
}
