#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

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
    class FreeAtOnceMemoryPool : public IMemoryPool
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        FreeAtOnceMemoryPool() = delete;

        FreeAtOnceMemoryPool(MemoryManager&     memoryManager,
                             std::string        poolName,
                             VkBufferUsageFlags bufferUsage,
                             VmaMemoryUsage     memoryUsage,
                             size_t             blockSize,
                             size_t             minBlocks,
                             size_t             maxBlocks);

        FreeAtOnceMemoryPool(const FreeAtOnceMemoryPool&) = delete;

        FreeAtOnceMemoryPool(FreeAtOnceMemoryPool&&) noexcept = delete;

        ~FreeAtOnceMemoryPool() noexcept override;

        FreeAtOnceMemoryPool& operator=(const FreeAtOnceMemoryPool&) = delete;

        FreeAtOnceMemoryPool& operator=(FreeAtOnceMemoryPool&&) noexcept = delete;

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

        std::mutex mutex;

        size_t deallocCount = 0;
    };
}  // namespace sol
