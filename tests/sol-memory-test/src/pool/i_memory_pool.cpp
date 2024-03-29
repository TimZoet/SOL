#include "sol-memory-test/pool/i_memory_pool.h"

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
        constexpr sol::IMemoryPool::CreateInfo info{.createFlags          = 0,
                                                    .bufferUsage          = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                    .requiredMemoryFlags  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                    .preferredMemoryFlags = 0,
                                                    .allocationFlags      = 0,
                                                    .blockSize            = 1024ull * 1024ull,
                                                    .minBlocks            = 0,
                                                    .maxBlocks            = 1};
        pool = &memoryManager->createFreeAtOnceMemoryPool("pool", info);
    });

    compareEQ(sol::IMemoryPool::Capabilities::None, pool->getCapabilities());

    // Try base allocation method.
    expectNoThrow([&] {
        constexpr sol::IBufferAllocator::AllocationInfo alloc{.size                = 1024ull,
                                                              .bufferUsage         = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                              .sharingMode         = VK_SHARING_MODE_EXCLUSIVE,
                                                              .memoryUsage         = VMA_MEMORY_USAGE_AUTO,
                                                              .requiredMemoryFlags = 0,
                                                              .preferredMemoryFlags = 0,
                                                              .allocationFlags      = 0,
                                                              .alignment            = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Allocate with invalid buffer usage.
    expectThrow([&] {
        constexpr sol::IBufferAllocator::AllocationInfo alloc{.size        = 1024ull,
                                                              .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                              .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
                                                              .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                              .requiredMemoryFlags  = 0,
                                                              .preferredMemoryFlags = 0,
                                                              .allocationFlags      = 0,
                                                              .alignment            = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Allocate with invalid memory flags.
    expectThrow([&] {
        constexpr sol::IBufferAllocator::AllocationInfo alloc{.size                = 1024ull,
                                                              .bufferUsage         = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                              .sharingMode         = VK_SHARING_MODE_EXCLUSIVE,
                                                              .memoryUsage         = VMA_MEMORY_USAGE_AUTO,
                                                              .requiredMemoryFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                                                              .preferredMemoryFlags = 0,
                                                              .allocationFlags      = 0,
                                                              .alignment            = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Aligned allocation is not supported yet.
    expectThrow([&] {
        constexpr sol::IBufferAllocator::AllocationInfo alloc{.size                = 1024ull,
                                                              .bufferUsage         = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                              .sharingMode         = VK_SHARING_MODE_EXCLUSIVE,
                                                              .memoryUsage         = VMA_MEMORY_USAGE_AUTO,
                                                              .requiredMemoryFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                                                              .preferredMemoryFlags = 0,
                                                              .allocationFlags      = 0,
                                                              .alignment            = 128};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
}
