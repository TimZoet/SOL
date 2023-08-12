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
        // Types.
        ////////////////////////////////////////////////////////////////

        struct CreateInfo
        {
            /**
             * \brief Pool create flags.
             */
            VmaPoolCreateFlags createFlags = 0;

            /**
             * \brief Buffer usage flags.
             */
            VkBufferUsageFlags bufferUsage = 0;

            /**
             * \brief Memory usage flags.
             */
            VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

            /**
             * \brief Required memory property flags.
             */
            VkMemoryPropertyFlags requiredMemoryFlags = 0;

            /**
             * \brief Preferred memory property flags.
             */
            VkMemoryPropertyFlags preferredMemoryFlags = 0;

            /**
             * \brief Allocation create flags.
             */
            VmaAllocationCreateFlags allocationFlags = 0;

            /**
             * \brief Size of memory blocks in bytes.
             */
            size_t blockSize = 0;

            /**
             * \brief Minimum number of memory blocks.If > 0, these blocks are preallocated.
             */
            size_t minBlocks = 0;

            /**
             * \brief Maximum number of memory blocks.
             */
            size_t maxBlocks = 0;
        };

        struct AllocationInfo
        {
            /**
             * \brief Size of buffer in bytes.
             */
            size_t size = 0;

            /**
             * \brief Buffer usage flags. If left at 0, the buffer usage flags with which the pool was created are used.
             */
            VkBufferUsageFlags bufferUsage = 0;

            /**
             * \brief Alignment in bytes.
             */
            size_t alignment = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IMemoryPool() = delete;

        IMemoryPool(MemoryManager&      memoryManager,
                    std::string         poolName,
                    CreateInfo          createInfo,
                    VulkanMemoryPoolPtr memoryPool);

        IMemoryPool(const IMemoryPool&) = delete;

        IMemoryPool(IMemoryPool&&) noexcept = delete;

        ~IMemoryPool() noexcept override;

        IMemoryPool& operator=(const IMemoryPool&) = delete;

        IMemoryPool& operator=(IMemoryPool&&) noexcept = delete;

        [[nodiscard]] static VulkanMemoryPoolPtr create(VulkanMemoryAllocator& allocator, const CreateInfo& info);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VmaPoolCreateFlags getCreateFlags() const noexcept;

        [[nodiscard]] VkBufferUsageFlags getBufferUsage() const noexcept;

        [[nodiscard]] VmaMemoryUsage getMemoryUsage() const noexcept;

        [[nodiscard]] VkMemoryPropertyFlags getRequiredMemoryFlags() const noexcept;

        [[nodiscard]] VkMemoryPropertyFlags getPreferredMemoryFlags() const noexcept;

        [[nodiscard]] VmaAllocationCreateFlags getAllocationFlags() const noexcept;

        [[nodiscard]] size_t getBlockSize() const noexcept;

        [[nodiscard]] size_t getMinBlocks() const noexcept;

        [[nodiscard]] size_t getMaxBlocks() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Allocations.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Allocate a new buffer from this memory pool.
         * \param alloc Allocation info.
         * \param onFailure Action to take when allocation fails.
         * \return Buffer.
         */
        [[nodiscard]] MemoryPoolBufferPtr allocateBuffer(AllocationInfo alloc, OnAllocationFailure onFailure);

        /**
         * \brief Allocate a new buffer from this memory pool. Calls allocateBuffer(AllocationInfo) with all parameters except size left at default.
         * \param size Buffer size in bytes.
         * \param onFailure Action to take when allocation fails.
         * \return Buffer.
         */
        [[nodiscard]] MemoryPoolBufferPtr allocateBuffer(size_t size, OnAllocationFailure onFailure);

    protected:
        [[nodiscard]] IBufferPtr allocateBufferImpl(const IBufferAllocator::AllocationInfo& alloc,
                                                    OnAllocationFailure                     onFailure) override;

        /**
         * \brief Allocate a new buffer from this memory pool.
         * \param alloc Allocation info.
         * \param onFailure Action to take when allocation fails.
         * \return Buffer.
         */
        virtual [[nodiscard]] std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
          allocateMemoryPoolBufferImpl(const AllocationInfo& alloc, OnAllocationFailure onFailure) = 0;

        /**
         * \brief Clean up resources associated with specified buffer. Called by MemoryPoolBuffer on destruction.
         * \param buffer Buffer.
         */
        virtual void releaseBuffer(const MemoryPoolBuffer& buffer) = 0;

    private:
        [[nodiscard]] MemoryPoolBufferPtr allocateMemoryPoolBuffer(const AllocationInfo& alloc,
                                                                   OnAllocationFailure   onFailure);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string name;

        CreateInfo info;

    protected:
        VulkanMemoryPoolPtr pool;
    };
}  // namespace sol
