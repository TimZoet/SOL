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

namespace sol
{
    class IMemoryPool
    {
    public:
        friend class MemoryPoolBuffer;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Additional capabilities that can be supported by memory pool implementations.
         * Certain methods can only be called if pools advertise support for them. Performing
         * unsupported operations will result in runtime errors.
         */
        enum class Capabilities : uint32_t
        {
            None = 0,

            /**
             * \brief Memory pool supports defragmentation.
             */
            Defragmentation = 1,

            /**
             * \brief Memory pool supports awaiting deallocation of other buffers
             * when trying to allocate a new buffer for which there is not enough space.
             */
            Wait = 2,

            /**
             * \brief Memory pool supports suballocation of buffers.
             */
            SubAllocation = 4
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IMemoryPool() = delete;

        IMemoryPool(MemoryManager&     memoryManager,
                    std::string        poolName,
                    VmaPoolCreateFlags createFlags,
                    VkBufferUsageFlags bufUsage,
                    VmaMemoryUsage     memUsage,
                    size_t             blckSize,
                    size_t             minBlcks,
                    size_t             maxBlcks);

        IMemoryPool(const IMemoryPool&) = delete;

        IMemoryPool(IMemoryPool&&) noexcept = delete;

        virtual ~IMemoryPool() noexcept;

        IMemoryPool& operator=(const IMemoryPool&) = delete;

        IMemoryPool& operator=(IMemoryPool&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        [[nodiscard]] VmaPoolCreateFlags getCreateFlags() const noexcept;

        [[nodiscard]] VkBufferUsageFlags getBufferUsage() const noexcept;

        [[nodiscard]] VmaMemoryUsage getMemoryUsage() const noexcept;

        [[nodiscard]] size_t getBlockSize() const noexcept;

        [[nodiscard]] size_t getMinBlocks() const noexcept;

        [[nodiscard]] size_t getMaxBlocks() const noexcept;

        [[nodiscard]] virtual Capabilities getCapabilities() const noexcept = 0;

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
        virtual [[nodiscard]] std::expected<MemoryPoolBufferPtr, std::unique_ptr<std::latch>>
          allocateBufferImpl(size_t size, bool waitOnOutOfMemory) = 0;

    private:
        [[nodiscard]] MemoryPoolBufferPtr allocateBuffer(size_t size, bool waitOnOutOfMemory);

        virtual void releaseBuffer(const MemoryPoolBuffer& buffer) = 0;

        ////////////////////////////////////////////////////////////////
        // Initialization.
        ////////////////////////////////////////////////////////////////

        void initialize();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* manager = nullptr;

        std::string name;

        VmaPoolCreateFlags flags = 0;

        VkBufferUsageFlags bufferUsage;

        VmaMemoryUsage memoryUsage;

        size_t blockSize;

        size_t minBlocks;

        size_t maxBlocks;

    protected:
        VulkanMemoryPoolPtr pool;
    };
}  // namespace sol
