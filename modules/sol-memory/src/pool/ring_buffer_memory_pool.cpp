#include "sol-memory/pool/ring_buffer_memory_pool.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_memory_pool.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"
#include "sol-memory/pool/memory_pool_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RingBufferMemoryPool::RingBufferMemoryPool(MemoryManager&      memoryManager,
                                               std::string         poolName,
                                               const CreateInfo&   createInfo,
                                               VulkanMemoryPoolPtr memoryPool) :
        IMemoryPool(memoryManager, std::move(poolName), createInfo, std::move(memoryPool))
    {
    }

    RingBufferMemoryPool::~RingBufferMemoryPool() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMemoryPool::Capabilities RingBufferMemoryPool::getCapabilities() const noexcept { return Capabilities::Wait; }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    void RingBufferMemoryPool::releaseBuffer(const MemoryPoolBuffer& buffer)
    {
        std::scoped_lock lock(mutex);

        assert(&buffer.getMemoryPool() == this);
        assert(buffer.getId() < buffers.size());
        assert(buffers[buffer.getId()]);

        buffers[buffer.getId()].reset();

        // Signal all threads that are waiting they can try again.
        for (const auto& latch : latches) latch->count_down();
        latches.clear();
    }

    std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
      RingBufferMemoryPool::allocateMemoryPoolBufferImpl(const AllocationInfo&     alloc,
                                                         const OnAllocationFailure onFailure)
    {
        std::scoped_lock lock(mutex);

        VulkanBuffer::Settings settings;
        settings.device        = getDevice();
        settings.size          = alloc.size;
        settings.bufferUsage   = alloc.bufferUsage;
        settings.allocator     = getMemoryManager().getAllocator();
        settings.vma.pool      = pool;
        settings.vma.flags     = getAllocationFlags();
        settings.vma.alignment = alloc.alignment;

        // Look for empty spot.
        size_t id = 0;
        for (; id < buffers.size(); id++)
        {
            if (!buffers[id]) break;
        }

        // No empty spot found, construct at end.
        if (id == buffers.size()) buffers.resize(id + 1);

        // Try to allocate buffer. If allocation fails and waitOnOutOfMemory is false, this will automatically throw.
        auto buffer = VulkanBuffer::create(settings, onFailure == OnAllocationFailure::Throw);

        // Allocation failed because pool is out of memory. Return nullptr or a std::latch that is signalled when a buffer is released.
        if (!buffer)
        {
            if (onFailure == OnAllocationFailure::Empty) return nullptr;

            auto latch = std::make_unique<std::latch>(2);
            latches.emplace_back(latch.get());
            return std::unexpected(std::move(latch));
        }

        buffers[id] = std::move(buffer);
        return std::make_unique<MemoryPoolBuffer>(*this, getDefaultQueueFamily(), id, *buffers[id], alloc.size, 0);
    }
}  // namespace sol
