#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
#include <vector>

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

namespace sol
{
    class BufferTransaction
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class BarrierLocation : uint32_t
        {
            /**
             * \brief Place barrier before copies.
             */
            BeforeCopy = 0,

            /**
             * \brief Place barrier after copies.
             */
            AfterCopy = 1
        };

        /**
         * \brief Describes a memory barrier and optional queue family transfer.
         */
        struct MemoryBarrier
        {
            /**
             * \brief Buffer.
             */
            IBuffer& buffer;

            /**
             * \brief Destination queue family. If not null, the buffer will be owned by dstFamily after the barrier.
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

        /**
         * \brief Describes a copy from an automatically allocated staging buffer to a destination buffer.
         */
        struct StagingBufferCopy
        {
            /**
             * \brief Destination buffer.
             */
            IBuffer& dstBuffer;

            /**
             * \brief Pointer to data.
             */
            const void* data = nullptr;

            /**
             * \brief Size of the copy in bytes. If set to VK_WHOLE_SIZE, dstBuffer.getBufferSize() is used.
             */
            size_t size = VK_WHOLE_SIZE;

            /**
             * \brief Offset into destination buffer. Is added to dstBuffer.getBufferOffset() if it is a suballocation.
             */
            size_t offset = 0;

            /**
             * \brief If there is an explicit memory barrier, transfer ownership of the destination buffer to the
             * transfer queue before doing the copy.
             * -
             *
             * If no explicit barrier with a destination queue is specified, ownership will go from the current owner
             * to the transfer queue before the copy, and back to the current owner after the copy.
             * -
             *
             * With an explicit destination queue, ownership will go from the current owner to the transfer queue
             * before the copy, and to the destination queue after the copy.
             */
            bool dstOnDedicatedTransfer = false;
        };

        /**
         * \brief Describes a copy from a source buffer to a destination buffer.
         */
        struct BufferToBufferCopy
        {
            /**
             * \brief Source buffer.
             */
            IBuffer& srcBuffer;

            /**
             * \brief Destination buffer.
             */
            IBuffer& dstBuffer;

            /**
             * \brief Size of the copy in bytes. If set to VK_WHOLE_SIZE, srcBuffer.getBufferSize() is used.
             */
            size_t size = VK_WHOLE_SIZE;

            /**
             * \brief Offset into source buffer. Is added to srcBuffer.getBufferOffset() if it is a suballocation.
             */
            size_t srcOffset = 0;

            /**
             * \brief Offset into destination buffer. Is added to dstBuffer.getBufferOffset() if it is a suballocation.
             */
            size_t dstOffset = 0;

            /**
             * \brief If there is an explicit memory barrier, transfer ownership of the source buffer to the transfer
             * queue before doing the copy.
             * -
             *
             * If no explicit destination queue is specified in the barrier, ownership will go from the current owner
             * to the transfer queue before the copy, and back to the current owner after the copy.
             * -
             *
             * With an explicit destination queue, ownership will go from the current owner to the transfer queue
             * before the copy, and to the destination queue after the copy.
             */
            bool srcOnDedicatedTransfer = false;

            /**
             * \brief If there is an explicit memory barrier, transfer ownership of the destination buffer to the
             * transfer queue before doing the copy.
             * -
             *
             * If no explicit barrier with a destination queue is specified, ownership will go from the current owner
             * to the transfer queue before the copy, and back to the current owner after the copy.
             * -
             *
             * With an explicit destination queue, ownership will go from the current owner to the transfer queue
             * before the copy, and to the destination queue after the copy.
             */
            bool dstOnDedicatedTransfer = false;
        };

        /**
         * \brief At any time, only 1 of the 4 optional values is valid.
         */
        struct Element
        {
            std::optional<MemoryBarrier>      preBarrier;
            std::optional<MemoryBarrier>      postBarrier;
            std::optional<StagingBufferCopy>  s2bCopy;
            std::optional<BufferToBufferCopy> b2bCopy;
            IBufferPtr                        stagingBuffer;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        BufferTransaction() = delete;

        explicit BufferTransaction(TransferManager& transferManager);

        BufferTransaction(const BufferTransaction&) = delete;

        BufferTransaction(BufferTransaction&&) noexcept = default;

        ~BufferTransaction() noexcept;

        BufferTransaction& operator=(const BufferTransaction&) = delete;

        BufferTransaction& operator=(BufferTransaction&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        [[nodiscard]] TransferManager& getTransferManager() noexcept;

        [[nodiscard]] const TransferManager& getTransferManager() const noexcept;

        /**
         * \brief Get the final semaphore value for each queue family that is set when the submitted command buffer completes.
         * Can be supplied to other submits that need to wait on this transaction to complete.
         * \return List of semaphore values. Can be indexed using queue family index.
         */
        [[nodiscard]] const std::vector<uint64_t>& getSemaphoreValues() const;

        ////////////////////////////////////////////////////////////////
        // Staging.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Stage a barrier.
         * \param barrier Barrier.
         * \param location Where to place the barrier, if a separate copy on the same buffer is being staged.
         */
        void stage(MemoryBarrier barrier, BarrierLocation location);

        /**
         * \brief Stage a copy from a pointer to a buffer. Optionally places a memory barrier around the copy.
         * -
         *
         * Will try to allocate an intermediate staging buffer. If allocation succeeds, a memcpy is performed
         * immediately and the data pointer can be released directly after this call. If allocation fails, this
         * transaction must be committed before doing any additional copies. Note that more memory barriers and
         * buffer-to-buffer copies can still be staged regardless of the outcome of this call, since they do not
         * require any staging buffer allocations.
         * -
         *
         * If there is no explicit barrier, it is assumed that manually placed barriers before and/or after the copy
         * will take care of any required synchronization. No automatic barriers are placed.
         * -
         *
         * With an explicit barrier, the supplied parameters are used to place two barriers around the copy command.
         * The before barrier takes the barrier.src values for the first scope and the transfer stage as the second
         * scope. The after barrier takes the transfer stage as the first scope and the barrier.dst values for the
         * second scope. Note that if the copy is only for part of the buffer, both barriers still apply to the whole
         * buffer.
         *
         * \param copy Copy info.
         * \param barrier Optional explicit barrier placed around the copy command.
         * \param waitOnAllocFailure If true and staging buffer allocation fails, wait on previous transaction(s)
         * to complete so that they release their staging buffers, and try allocating again.
         * \return Staging buffer allocation success. If allocation fails, false is returned and this transaction must
         * be committed before doing any additional copies. Note that more memory barriers and buffer-to-buffer
         * copies can still be added regardless of the outcome of this call, since they do not require any staging
         * buffer allocations.
         */
        [[nodiscard]] bool stage(const StagingBufferCopy& copy, const std::optional<MemoryBarrier>& barrier = {}, bool waitOnAllocFailure = false);

        /**
         * \brief Stage a copy from a source buffer to a destination buffer. Optionally places barriers around the
         * copy.
         * -
         *
         * If there are no explicit barriers, it is assumed that manually placed barriers before and/or after the copy
         * will take care of any required synchronization. No automatic barriers are placed.
         * -
         *
         * With explicit barriers, the supplied parameters are used to place two barriers around the copy command for
         * the source and/or destination buffer. The before barrier takes the barrier.src values for the first scope
         * and the transfer stage as the second scope. The after barrier takes the transfer stage as the first scope
         * and the barrier.dst values for the second scope. Note that if the copy is only for part of the buffer, both
         * barriers still apply to the whole buffer.
         * \param copy Copy.
         * \param srcBarrier Optional explicit memory barrier for the source buffer.
         * \param dstBarrier Optional explicit memory barrier for the destination buffer.
         */
        void stage(const BufferToBufferCopy&           copy,
                   const std::optional<MemoryBarrier>& srcBarrier = {},
                   const std::optional<MemoryBarrier>& dstBarrier = {});

        ////////////////////////////////////////////////////////////////
        // Commit.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Commit this transaction. Will wait for previously committed transactions to complete
         * and then record and submit command buffers.
         */
        void commit();

        /**
         * \brief Do a CPU-side wait on the semaphores. Can only be called after committing.
         * For GPU-side waiting, you can directly retrieve the final state of the semaphores using getSemaphoreValues().
         */
        void wait();

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        TransferManager* manager = nullptr;

        size_t index = 0;

        std::vector<Element> staged;

        bool committed = false;

        bool done = false;

        std::vector<uint64_t> semaphoreValues;
    };
}  // namespace sol
