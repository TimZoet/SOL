#include "sol-memory/pool/free_at_once_memory_pool.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_memory_pool.h"
#include "sol-error/sol_error.h"

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

    FreeAtOnceMemoryPool::FreeAtOnceMemoryPool(MemoryManager&      memoryManager,
                                               std::string         poolName,
                                               const CreateInfo&   createInfo,
                                               VulkanMemoryPoolPtr memoryPool) :
        IMemoryPool(memoryManager, std::move(poolName), createInfo, std::move(memoryPool))
    {
    }

    FreeAtOnceMemoryPool::~FreeAtOnceMemoryPool() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMemoryPool::Capabilities FreeAtOnceMemoryPool::getCapabilities() const noexcept { return Capabilities::None; }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    void FreeAtOnceMemoryPool::releaseBuffer(const MemoryPoolBuffer& buffer)
    {
        std::scoped_lock lock(mutex);

        assert(&buffer.getMemoryPool() == this);
        assert(buffer.getId() < buffers.size());
        assert(buffers[buffer.getId()]);

        buffers[buffer.getId()].reset();

        // Keep track of the number of deallocated buffers.
        deallocCount++;

        // If all buffers have been deallocated, reset.
        if (deallocCount == buffers.size())
        {
            deallocCount = 0;
            buffers.clear();
        }
    }

    std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
      FreeAtOnceMemoryPool::allocateMemoryPoolBufferImpl(const AllocationInfo&     alloc,
                                                         const OnAllocationFailure onFailure)
    {
        std::scoped_lock lock(mutex);

        if (deallocCount)
            throw SolError("Cannot allocate new buffer from FreeAtOnceMemoryPool before all previous buffers have been "
                           "deallocated.");

        VulkanBuffer::Settings settings;
        settings.device        = getDevice();
        settings.size          = alloc.size;
        settings.bufferUsage   = alloc.bufferUsage;
        settings.allocator     = getMemoryManager().getAllocator();
        settings.vma.pool      = pool;
        settings.vma.flags     = getAllocationFlags();
        settings.vma.alignment = alloc.alignment;

        auto buffer = VulkanBuffer::create(settings, onFailure != OnAllocationFailure::Empty);
        if (!buffer) return nullptr;
        auto& b = *buffers.emplace_back(std::move(buffer));
        return std::make_unique<MemoryPoolBuffer>(*this, getDefaultQueueFamily(), buffers.size() - 1, b, alloc.size, 0);
    }
}  // namespace sol
