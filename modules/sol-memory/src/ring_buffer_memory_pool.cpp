#include "sol-memory/ring_buffer_memory_pool.h"

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
#include "sol-memory/memory_pool_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RingBufferMemoryPool::RingBufferMemoryPool(MemoryManager&           memoryManager,
                                               std::string              poolName,
                                               const VkBufferUsageFlags bufferUsage,
                                               const VmaMemoryUsage     memoryUsage,
                                               const size_t             blockSize,
                                               const bool               preallocate) :
        IMemoryPool(memoryManager,
                    std::move(poolName),
                    VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT,
                    bufferUsage,
                    memoryUsage,
                    blockSize,
                    preallocate ? 1 : 0,
                    1)
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
      RingBufferMemoryPool::allocateBufferImpl(const size_t size, const bool waitOnOutOfMemory)
    {
        std::scoped_lock lock(mutex);

        VulkanBuffer::Settings settings;
        settings.device    = getDevice();
        settings.size      = size;
        settings.allocator = getMemoryManager().getAllocator();
        settings.vma.pool  = pool;

        // Look for empty spot.
        size_t id = 0;
        for (; id < buffers.size(); id++)
        {
            if (!buffers[id]) break;
        }

        // No empty spot found, construct at end.
        if (id == buffers.size()) buffers.resize(id + 1);

        // Try to allocate buffer. If allocation fails and waitOnOutOfMemory is false, this will automatically throw.
        auto buffer = VulkanBuffer::create(settings, !waitOnOutOfMemory);

        // Allocation failed because pool is out of memory. Return a std::latch that is signalled when a buffer is released.
        if (!buffer)
        {
            auto latch = std::make_unique<std::latch>(2);
            latches.emplace_back(latch.get());
            return std::unexpected(std::move(latch));
        }

        buffers[id] = std::move(buffer);
        return std::make_unique<MemoryPoolBuffer>(*this, id, *buffers[id], size, 0);
    }
}  // namespace sol
