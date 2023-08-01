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
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/pool/i_memory_pool.h"

namespace sol
{
    class RingBufferMemoryPool : public IMemoryPool
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RingBufferMemoryPool() = delete;

        RingBufferMemoryPool(MemoryManager&        memoryManager,
                             std::string           poolName,
                             VkBufferUsageFlags    bufferUsage,
                             VmaMemoryUsage        memoryUsage,
                             VkMemoryPropertyFlags requiredMemFlags,
                             VkMemoryPropertyFlags preferredMemFlags,
                             size_t                blockSize,
                             bool                  preallocate);

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
        // Allocations.
        ////////////////////////////////////////////////////////////////

    protected:
        [[nodiscard]] std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
          allocateMemoryPoolBufferImpl(size_t size, bool waitOnOutOfMemory) override;

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
