#include "sol-memory/pool/non_linear_memory_pool.h"

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

    NonLinearMemoryPool::NonLinearMemoryPool(MemoryManager&      memoryManager,
                                             std::string         poolName,
                                             const CreateInfo&   createInfo,
                                             VulkanMemoryPoolPtr memoryPool) :
        IMemoryPool(memoryManager, std::move(poolName), createInfo, std::move(memoryPool))
    {
    }

    NonLinearMemoryPool::~NonLinearMemoryPool() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMemoryPool::Capabilities NonLinearMemoryPool::getCapabilities() const noexcept
    {
        // TODO: This pool should also support defragging.
        return Capabilities::None;
    }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    void NonLinearMemoryPool::releaseBuffer(const MemoryPoolBuffer& buffer)
    {
        std::scoped_lock lock(mutex);

        assert(&buffer.getMemoryPool() == this);
        assert(buffer.getId() < buffers.size());
        assert(buffers[buffer.getId()]);
        buffers[buffer.getId()].reset();
    }

    std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
      NonLinearMemoryPool::allocateMemoryPoolBufferImpl(const AllocationInfo& alloc, const bool)
    {
        std::scoped_lock lock(mutex);

        VulkanBuffer::Settings settings;
        settings.device        = getDevice();
        settings.size          = alloc.size;
        settings.bufferUsage   = alloc.bufferUsage;
        settings.allocator     = getMemoryManager().getAllocator();
        settings.vma.pool      = pool;
        settings.vma.alignment = alloc.alignment;

        // Look for empty spot.
        size_t id = 0;
        for (; id < buffers.size(); id++)
        {
            if (!buffers[id]) break;
        }

        // No empty spot found, construct at end.
        if (id == buffers.size())
        {
            auto& buffer = *buffers.emplace_back(VulkanBuffer::create(settings));
            return std::make_unique<MemoryPoolBuffer>(*this, id, buffer, alloc.size, 0);
        }

        // Fill in empty spot.
        buffers[id] = VulkanBuffer::create(settings);
        return std::make_unique<MemoryPoolBuffer>(*this, id, *buffers[id], alloc.size, 0);
    }
}  // namespace sol
