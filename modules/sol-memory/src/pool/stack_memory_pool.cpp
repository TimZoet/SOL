#include "sol-memory/pool/stack_memory_pool.h"

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

    StackMemoryPool::StackMemoryPool(MemoryManager&      memoryManager,
                                     std::string         poolName,
                                     const CreateInfo&   createInfo,
                                     VulkanMemoryPoolPtr memoryPool) :
        IMemoryPool(memoryManager, std::move(poolName), createInfo, std::move(memoryPool))
    {
    }

    StackMemoryPool::~StackMemoryPool() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMemoryPool::Capabilities StackMemoryPool::getCapabilities() const noexcept { return Capabilities::None; }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    void StackMemoryPool::releaseBuffer(const MemoryPoolBuffer& buffer)
    {
        std::scoped_lock lock(mutex);

        assert(&buffer.getMemoryPool() == this);
        assert(buffer.getId() < buffers.size());
        assert(buffers[buffer.getId()]);

        buffers[buffer.getId()].reset();

        // If this was not the last allocated buffer, it is being deallocated in the wrong order.
        if (buffer.getId() + 1 != currentIndex) invalidOrder = true;

        currentIndex--;

        // If all buffers at index [currentIndex, buffers.size()) have been deallocated,
        // there sort of is a valid deallocation order again.
        if (invalidOrder)
        {
            bool clear = true;
            for (size_t i = currentIndex; i < buffers.size(); i++)
            {
                if (buffers[i])
                {
                    clear = false;
                    break;
                }
            }

            if (clear) invalidOrder = false;
        }
    }

    std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
      StackMemoryPool::allocateMemoryPoolBufferImpl(const size_t size, const bool)
    {
        std::scoped_lock lock(mutex);

        // Cannot allocate a new buffer until the deallocation order has been fixed.
        if (invalidOrder)
        {
            throw SolError("A buffer was deallocated in the wrong order. The StackMemoryPool requires buffers to be "
                           "allocated and deallocated in LIFO order.");
        }

        VulkanBuffer::Settings settings;
        settings.device    = getDevice();
        settings.size      = size;
        settings.allocator = getMemoryManager().getAllocator();
        settings.vma.pool  = pool;

        // Look for empty spot.
        if (currentIndex < buffers.size())
        {
            buffers[currentIndex++] = VulkanBuffer::create(settings);
            return std::make_unique<MemoryPoolBuffer>(*this, currentIndex - 1, *buffers[currentIndex - 1], size, 0);
        }

        // No empty spot found, construct at end.
        auto& buffer = *buffers.emplace_back(VulkanBuffer::create(settings));
        currentIndex = buffers.size();
        return std::make_unique<MemoryPoolBuffer>(*this, currentIndex - 1, buffer, size, 0);
    }
}  // namespace sol
