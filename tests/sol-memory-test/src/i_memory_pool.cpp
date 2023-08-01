#include "sol-memory-test/i_memory_pool.h"

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

void IMemoryPool::operator()()
{
    // NOTE: Always allocating just a little bit below what's indicated. Memory requirements for a buffer
    // tend to be higher since the driver adds some of its own data for bookkeeping.

    // NOTE: IMemoryPool is abstract, so using FreeAtOnceMemoryPool instead to test it.

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

    // Create a memory pool with 1 block of 1MiB.
    sol::FreeAtOnceMemoryPool* pool = nullptr;
    expectNoThrow([&] {
        pool = &memoryManager->createFreeAtOnceMemoryPool("pool",
                                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                          VMA_MEMORY_USAGE_AUTO,
                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                          0,
                                                          1024ull * 1024,
                                                          0,
                                                          1);
    });

    compareEQ(sol::IMemoryPool::Capabilities::None, pool->getCapabilities());

    // Try base allocation method.
    expectNoThrow([&] {
        constexpr sol::IBufferAllocator::Allocation alloc{.size                 = 1024ull,
                                                          .bufferUsage          = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
                                                          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                          .requiredMemoryFlags  = 0,
                                                          .preferredMemoryFlags = 0,
                                                          .allocationFlags      = 0};
        static_cast<void>(pool->allocateBuffer(alloc));
    });

    // Allocate with invalid buffer usage.
    expectThrow([&] {
        constexpr sol::IBufferAllocator::Allocation alloc{.size        = 1024ull,
                                                          .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
                                                          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                          .requiredMemoryFlags  = 0,
                                                          .preferredMemoryFlags = 0,
                                                          .allocationFlags      = 0};
        static_cast<void>(pool->allocateBuffer(alloc));
    });

    // Allocate with invalid memory flags.
    expectThrow([&] {
        constexpr sol::IBufferAllocator::Allocation alloc{.size                 = 1024ull,
                                                          .bufferUsage          = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
                                                          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                          .requiredMemoryFlags  = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                                                          .preferredMemoryFlags = 0,
                                                          .allocationFlags      = 0};
        static_cast<void>(pool->allocateBuffer(alloc));
    });

    // Aligned allocation is not supported yet.
    expectThrow([&] {
        constexpr sol::IBufferAllocator::AllocationAligned alloc{.size        = 1024ull,
                                                                 .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                                 .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                                 .memoryUsage = VMA_MEMORY_USAGE_AUTO,
                                                                 .requiredMemoryFlags =
                                                                   VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                                                                 .preferredMemoryFlags = 0,
                                                                 .allocationFlags      = 0,
                                                                 .alignment            = 128};
        static_cast<void>(pool->allocateBuffer(alloc));
    });
}
