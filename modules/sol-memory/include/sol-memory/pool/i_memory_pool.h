#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <expected>
#include <latch>
#include <string>

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

#include "sol-memory/fwd.h"
#include "sol-memory/i_buffer_allocator.h"

namespace sol
{
    class IMemoryPool : public IBufferAllocator
    {
    public:
        friend class MemoryPoolBuffer;

        using IBufferAllocator::allocateBuffer;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IMemoryPool() = delete;

        IMemoryPool(MemoryManager&        memoryManager,
                    std::string           poolName,
                    VmaPoolCreateFlags    createFlags,
                    VkBufferUsageFlags    bufUsage,
                    VmaMemoryUsage        memUsage,
                    VkMemoryPropertyFlags requiredMemFlags,
                    VkMemoryPropertyFlags preferredMemFlags,
                    size_t                blckSize,
                    size_t                minBlcks,
                    size_t                maxBlcks);

        IMemoryPool(const IMemoryPool&) = delete;

        IMemoryPool(IMemoryPool&&) noexcept = delete;

        ~IMemoryPool() noexcept override;

        IMemoryPool& operator=(const IMemoryPool&) = delete;

        IMemoryPool& operator=(IMemoryPool&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VmaPoolCreateFlags getCreateFlags() const noexcept;

        [[nodiscard]] VkBufferUsageFlags getBufferUsage() const noexcept;

        [[nodiscard]] VmaMemoryUsage getMemoryUsage() const noexcept;

        [[nodiscard]] VkMemoryPropertyFlags getRequiredMemoryFlags() const noexcept;

        [[nodiscard]] VkMemoryPropertyFlags getPreferredMemoryFlags() const noexcept;

        [[nodiscard]] size_t getBlockSize() const noexcept;

        [[nodiscard]] size_t getMinBlocks() const noexcept;

        [[nodiscard]] size_t getMaxBlocks() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Allocations.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Allocate a new buffer from this memory pool.
         * \param size Size of buffer in bytes.
         * \return Buffer.
         */
        [[nodiscard]] MemoryPoolBufferPtr allocateBuffer(size_t size);

        /**
         * \brief Allocate a new buffer from this memory pool. If pool is full, wait for deallocations that free up space.
         * Note that a wrong (de)allocation order, or not enough memory being available ever, can result in deadlocks.
         * \param size Size of buffer in bytes.
         * \return Buffer.
         */
        [[nodiscard]] MemoryPoolBufferPtr allocateBufferWithWait(size_t size);

    protected:
        [[nodiscard]] IBufferPtr allocateBufferImpl(const Allocation& alloc) override;

        [[nodiscard]] IBufferPtr allocateBufferImpl(const AllocationAligned& alloc) override;

        /**
         * \brief Allocate a new buffer from this memory pool.
         * \param size Size of buffer in bytes.
         * \param waitOnOutOfMemory If waiting is not supported, this value will never be true and can be ignored.
         * \return Buffer.
         */
        virtual [[nodiscard]] std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
          allocateMemoryPoolBufferImpl(size_t size, bool waitOnOutOfMemory) = 0;

        /**
         * \brief Clean up resources associated with specified buffer. Called by MemoryPoolBuffer on destruction.
         * \param buffer Buffer.
         */
        virtual void releaseBuffer(const MemoryPoolBuffer& buffer) = 0;

    private:
        [[nodiscard]] MemoryPoolBufferPtr allocateMemoryPoolBuffer(size_t size, bool waitOnOutOfMemory);

        ////////////////////////////////////////////////////////////////
        // Initialization.
        ////////////////////////////////////////////////////////////////

        void initialize();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string name;

        VmaPoolCreateFlags flags = 0;

        VkBufferUsageFlags bufferUsage;

        VmaMemoryUsage memoryUsage;

        VkMemoryPropertyFlags requiredMemoryFlags;

        VkMemoryPropertyFlags preferredMemoryFlags;

        size_t blockSize;

        size_t minBlocks;

        size_t maxBlocks;

    protected:
        VulkanMemoryPoolPtr pool;
    };
}  // namespace sol
