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
        constexpr sol::IMemoryPool::CreateInfo info{.createFlags          = 0,
                                                    .bufferUsage          = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                    .requiredMemoryFlags  = 0,
                                                    .preferredMemoryFlags = 0,
                                                    .allocationFlags      = 0,
                                                    .blockSize            = 1024ull * 1024ull,
                                                    .minBlocks            = 0,
                                                    .maxBlocks            = 2};
        pool = &memoryManager->createFreeAtOnceMemoryPool("pool", info);
    });

    compareEQ(sol::IMemoryPool::Capabilities::None, pool->getCapabilities());

    // Two allocations of half block size.
    std::vector<sol::MemoryPoolBufferPtr> buffers;
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
    compareEQ(1024ull * 512, buffers[0]->getBufferSize());
    compareEQ(1024ull * 512, buffers[1]->getBufferSize());

    // Allocation larger than block size.
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 2048ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Fill up remaining blocks.
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers.emplace_back(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Any new allocation should fail as long as previous allocations were not cleared.
    expectNoThrow([&] { buffers[0].reset(); });
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
    expectNoThrow([&] { buffers[1].reset(); });
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
    expectNoThrow([&] { buffers[2].reset(); });
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
    expectNoThrow([&] { buffers[3].reset(); });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
}
