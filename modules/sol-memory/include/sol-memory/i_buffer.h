#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/fwd.h"
#include "sol-memory/transaction.h"

namespace sol
{
    class IBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Barrier
        {
            /**
             * \brief Destination queue family. If not null, the buffer will be owned by dstFamily after the barrier. Otherwise, the owner will remain the same.
             */
            const VulkanQueueFamily* dstFamily = nullptr;

            /**
             * \brief Source stage.
             */
            VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_NONE_KHR;

            /**
             * \brief Destination stage.
             */
            VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_NONE_KHR;

            /**
             * \brief Source access.
             */
            VkAccessFlags2 srcAccess = VK_ACCESS_2_NONE;

            /**
             * \brief Destination access.
             */
            VkAccessFlags2 dstAccess = VK_ACCESS_2_NONE;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IBuffer() = delete;

        IBuffer(MemoryManager& memoryManager, const VulkanQueueFamily& family);

        IBuffer(const IBuffer&) = delete;

        IBuffer(IBuffer&&) noexcept = default;

        virtual ~IBuffer() noexcept;

        IBuffer& operator=(const IBuffer&) = delete;

        IBuffer& operator=(IBuffer&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        /**
         * \brief Get the queue family that currently owns this resource.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily& getQueueFamily() const noexcept;

        [[nodiscard]] virtual VulkanBuffer& getBuffer() = 0;

        [[nodiscard]] virtual const VulkanBuffer& getBuffer() const = 0;

        /**
         * \brief Get size of buffer in bytes.
         * \return Size in bytes.
         */
        [[nodiscard]] virtual size_t getBufferSize() const noexcept = 0;

        /**
         * \brief If a suballocation, offset into entire buffer in bytes.
         * \return Offset in bytes.
         */
        [[nodiscard]] virtual size_t getBufferOffset() const noexcept = 0;

        [[nodiscard]] virtual bool isSubAllocation() const noexcept = 0;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setQueueFamily(const VulkanQueueFamily& family) noexcept;

        ////////////////////////////////////////////////////////////////
        // Transactions.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Place a barrier.
         * \param transaction Transaction to append to.
         * \param barrier Barrier.
         * \param location Where to place the barrier, if a separate copy on the same buffer is being staged.
         */
        virtual void barrier(Transaction& transaction, const Barrier& barrier, BarrierLocation location);

        /**
         * \brief Set the buffer data in the range [offset, offset + size).
         * \param transaction Transaction to append to.
         * \param data Pointer to data. Will be copied into a staging buffer. Can be released directly after this call.
         * \param size Number of bytes.
         * \param offset Offset into buffer, in number of bytes.
         * \param barrier Barrier placed around the copy command. Transfer stage and access are automatically taken care of.
         * \param waitOnAllocFailure If true and staging buffer allocation fails, wait on previous transaction(s)
         * to complete so that they release their staging buffers, and try allocating again.
         * \return True if staging buffer allocation succeeded and transaction can be committed.
         * On failure, already staged transactions should be committed and waited on before trying again.
         */
        [[nodiscard]] virtual bool setData(Transaction&   transaction,
                                           const void*    data,
                                           size_t         size,
                                           size_t         offset,
                                           const Barrier& barrier,
                                           bool           waitOnAllocFailure);

        /**
         * \brief Get the buffer data in the range [srcOffset, srcOffset + count). Copies the data to destination buffer in the range [dstOffset, dstOffset + count).
         * \param transaction Transaction to append to.
         * \param dstBuffer Destination buffer.
         * \param srcBarrier Barrier placed around the copy command for the source buffer.
         * \param dstBarrier Barrier placed around the copy command for the destination buffer.
         * \param size Number of bytes to copy.
         * \param srcOffset Offset into the source buffer, in bytes.
         * \param dstOffset Offset into the destination buffer, in bytes.
         */
        virtual void getData(Transaction&   transaction,
                             IBuffer&       dstBuffer,
                             const Barrier& srcBarrier,
                             const Barrier& dstBarrier,
                             size_t         size,
                             size_t         srcOffset,
                             size_t         dstOffset);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* manager = nullptr;

        /**
         * \brief Queue family that currently owns this buffer.
         */
        const VulkanQueueFamily* queueFamily = nullptr;
    };
}  // namespace sol
