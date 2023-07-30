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

    FreeAtOnceMemoryPool::FreeAtOnceMemoryPool(MemoryManager&           memoryManager,
                                               std::string              poolName,
                                               const VkBufferUsageFlags bufferUsage,
                                               const VmaMemoryUsage     memoryUsage,
                                               const size_t             blockSize,
                                               const size_t             minBlocks,
                                               const size_t             maxBlocks) :
        IMemoryPool(memoryManager,
                    std::move(poolName),
                    VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT,
                    bufferUsage,
                    memoryUsage,
                    blockSize,
                    minBlocks,
                    maxBlocks)
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
      FreeAtOnceMemoryPool::allocateBufferImpl(const size_t size, const bool)
    {
        std::scoped_lock lock(mutex);

        if (deallocCount)
            throw SolError("Cannot allocate new buffer from FreeAtOnceMemoryPool before all previous buffers have been "
                           "deallocated.");

        VulkanBuffer::Settings settings;
        settings.device    = getDevice();
        settings.size      = size;
        settings.allocator = getMemoryManager().getAllocator();
        settings.vma.pool  = pool;

        auto& buffer = *buffers.emplace_back(VulkanBuffer::create(settings));
        return std::make_unique<MemoryPoolBuffer>(*this, buffers.size() - 1, buffer, size, 0);
    }
}  // namespace sol
