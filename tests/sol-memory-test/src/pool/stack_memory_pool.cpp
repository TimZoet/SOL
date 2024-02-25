#include "sol-memory-test/pool/stack_memory_pool.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/pool/memory_pool_buffer.h"
#include "sol-memory/pool/stack_memory_pool.h"

void StackMemoryPool::operator()()
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
    sol::StackMemoryPool* pool = nullptr;
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
        pool = &memoryManager->createStackMemoryPool("pool", info);
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

    // Clearing the last buffer should open up space again.
    expectNoThrow([&] { buffers[3].reset(); });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers[3] = pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);
    });

    // Clearing the third buffer opens up space, but in the wrong place.
    expectNoThrow([&] { buffers[2].reset(); });
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers[2] = pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);
    });

    // Clearing the fourth buffer opens up contiguous space at the correct offset again.
    expectNoThrow([&] { buffers[3].reset(); });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers[2] = pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);
    });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        buffers[3] = pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);
    });

    // Clear all memory.
    expectNoThrow([&] { buffers.clear(); });

    // Fill up memory.
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 256ull, .bufferUsage = 0, .alignment = 0};
        for (size_t i = 0; i < 8; i++)
            buffers.emplace_back(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
}
