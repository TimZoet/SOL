#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-memory/i_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class IndexBuffer final : public IBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IndexBuffer() = delete;

        IndexBuffer(GeometryBufferAllocator& alloc,
                    VulkanBufferPtr          buffer,
                    size_t                   idxCount,
                    size_t                   idxSize,
                    const VulkanQueueFamily& family);

        IndexBuffer(GeometryBufferAllocator& alloc,
                    IBuffer&                 buffer,
                    size_t                   idxCount,
                    size_t                   idxSize,
                    VmaVirtualAllocation     allocation,
                    size_t                   idxOffset);

        IndexBuffer(const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&&) = delete;

        ~IndexBuffer() noexcept override;

        IndexBuffer& operator=(const IndexBuffer&) = delete;

        IndexBuffer& operator=(IndexBuffer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] GeometryBufferAllocator& getAllocator() noexcept;

        [[nodiscard]] const GeometryBufferAllocator& getAllocator() const noexcept;

        [[nodiscard]] size_t getIndexCount() const noexcept;

        [[nodiscard]] size_t getIndexSize() const noexcept;

        /**
         * \brief Get the offset into global buffer in number of indices.
         * \return Offset in number of indices.
         */
        [[nodiscard]] size_t getIndexOffset() const noexcept;

        [[nodiscard]] VkIndexType getIndexType() const noexcept;

        [[nodiscard]] VulkanBuffer& getBuffer() override;

        [[nodiscard]] const VulkanBuffer& getBuffer() const override;

        /**
         * \brief Get size of buffer in bytes.
         * \return Size in bytes.
         */
        [[nodiscard]] size_t getBufferSize() const noexcept override;

        /**
         * \brief If a suballocation, offset into entire buffer in bytes.
         * \return Offset in bytes.
         */
        [[nodiscard]] size_t getBufferOffset() const noexcept override;

        [[nodiscard]] bool isSubAllocation() const noexcept override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////
        // Transactions.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the index data in the range [offset, offset + count).
         * \param transaction Transaction to append to.
         * \param data Pointer to data. Will be copied into a staging buffer. Can be released directly after this call.
         * \param count Number of indices in data.
         * \param offset Offset into index buffer, in number of indices.
         * \param barrier Barrier placed around the copy command. Transfer stage and access are automatically taken care of.
         * \param waitOnAllocFailure If true and staging buffer allocation fails, wait on previous transaction(s)
         * to complete so that they release their staging buffers, and try allocating again.
         * \return True if staging buffer allocation succeeded and transaction can be committed.
         * On failure, already staged transactions should be committed and waited on before trying again.
         */
        [[nodiscard]] bool setIndexData(Transaction&   transaction,
                                        const void*    data,
                                        size_t         count,
                                        size_t         offset,
                                        const Barrier& barrier,
                                        bool           waitOnAllocFailure);

        /**
         * \brief Get the index data in the range [srcOffset, srcOffset + count). Copies the data to a destination buffer.
         * Note: destination buffer is interpreted as an index buffer. Check the units of the count and offsets.
         * \param transaction Transaction to append to.
         * \param dstBuffer Destination buffer. Must be a index buffer with an equal index size.
         * \param srcBarrier Barrier placed around the copy command for the source buffer.
         * \param dstBarrier Barrier placed around the copy command for the destination buffer.
         * \param count Number of indices to copy.
         * \param srcOffset Offset into the source buffer, in number of indices.
         * \param dstOffset Offset into the destination buffer, in number of indices.
         */
        void getIndexData(Transaction&   transaction,
                          IndexBuffer&   dstBuffer,
                          const Barrier& srcBarrier,
                          const Barrier& dstBarrier,
                          size_t         count,
                          size_t         srcOffset,
                          size_t         dstOffset);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        GeometryBufferAllocator* allocator = nullptr;

        /**
         * \brief If allocated separately.
         */
        VulkanBufferPtr freeBuffer;

        /**
         * \brief If allocated from a global buffer.
         */
        IBuffer* globalBuffer = nullptr;

        /**
         * \brief Number of indices.
         */
        size_t indexCount = 0;

        /**
         * \brief Size of each index in bytes.
         */
        size_t indexSize = 0;

        /**
         * \brief Allocation into global buffer.
         */
        VmaVirtualAllocation virtualAllocation = VK_NULL_HANDLE;

        /**
         * \brief Offset into global buffer in number of indices.
         */
        size_t indexOffset = 0;
    };
}  // namespace sol
