#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <mutex>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-memory/i_memory_pool.h"

namespace sol
{
    class RingBufferMemoryPool : public IMemoryPool
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RingBufferMemoryPool() = delete;

        RingBufferMemoryPool(MemoryManager&     memoryManager,
                             std::string        poolName,
                             VkBufferUsageFlags bufferUsage,
                             VmaMemoryUsage     memoryUsage,
                             size_t             blockSize,
                             bool               preallocate);

        RingBufferMemoryPool(const RingBufferMemoryPool&) = delete;

        RingBufferMemoryPool(RingBufferMemoryPool&&) noexcept = delete;

        ~RingBufferMemoryPool() noexcept override;

        RingBufferMemoryPool& operator=(const RingBufferMemoryPool&) = delete;

        RingBufferMemoryPool& operator=(RingBufferMemoryPool&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Capabilities getCapabilities() const noexcept override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////
        // Allocations.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
          allocateBufferImpl(size_t size, bool waitOnOutOfMemory) override;

    private:
        void releaseBuffer(const MemoryPoolBuffer& buffer) override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<VulkanBufferPtr> buffers;

        std::vector<std::latch*> latches;

        std::mutex mutex;
    };
}  // namespace sol
