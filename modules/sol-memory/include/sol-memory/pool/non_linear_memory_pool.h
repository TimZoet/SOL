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
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/pool/i_memory_pool.h"

namespace sol
{
    class NonLinearMemoryPool : public IMemoryPool
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        NonLinearMemoryPool() = delete;

        NonLinearMemoryPool(MemoryManager&        memoryManager,
                            std::string           poolName,
                            VkBufferUsageFlags    bufferUsage,
                            VmaMemoryUsage        memoryUsage,
                            VkMemoryPropertyFlags requiredMemFlags,
                            VkMemoryPropertyFlags preferredMemFlags,
                            size_t                blockSize,
                            size_t                minBlocks,
                            size_t                maxBlocks);

        NonLinearMemoryPool(const NonLinearMemoryPool&) = delete;

        NonLinearMemoryPool(NonLinearMemoryPool&&) noexcept = delete;

        ~NonLinearMemoryPool() noexcept override;

        NonLinearMemoryPool& operator=(const NonLinearMemoryPool&) = delete;

        NonLinearMemoryPool& operator=(NonLinearMemoryPool&&) noexcept = delete;

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

        std::mutex mutex;
    };
}  // namespace sol