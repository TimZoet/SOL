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
        constexpr sol::IMemoryPool::CreateInfo info{.createFlags          = 0,
                                                    .bufferUsage          = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                    .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                    .requiredMemoryFlags  = 0,
                                                    .preferredMemoryFlags = 0,
                                                    .allocationFlags      = 0,
                                                    .blockSize            = 1024ull * 1024ull,
                                                    .minBlocks            = 0,
                                                    .maxBlocks            = 10};
        pool = &memoryManager->createNonLinearMemoryPool("pool", info);
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
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 1024ull, .bufferUsage = 0, .alignment = 0};
        for (size_t i = 0; i < 9; i++) buffers.emplace_back(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Out of memory.
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 128ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // This pool type does not support waiting.
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Wait));
    });

    // Clear up some space.
    expectNoThrow([&] { buffers[0].reset(); });

    // Still out of memory because buffer[0] used only half a block.
    expectThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 1024ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // But this should work again.
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 512ull, .bufferUsage = 0, .alignment = 0};
        static_cast<void>(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });

    // Clear out everything and then fill the whole thing again with a bunch of small allocations.
    expectNoThrow([&] { buffers.clear(); });
    expectNoThrow([&] {
        constexpr sol::IMemoryPool::AllocationInfo alloc{.size = 1024ull * 32ull, .bufferUsage = 0, .alignment = 0};
        for (size_t i = 0; i < 320; i++) buffers.emplace_back(pool->allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw));
    });
}
