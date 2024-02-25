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
#include "sol-memory/transaction.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class VertexBuffer final : public IBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VertexBuffer() = delete;

        VertexBuffer(GeometryBufferAllocator& alloc,
                     VulkanBufferPtr          buffer,
                     size_t                   vtxCount,
                     size_t                   vtxSize,
                     const VulkanQueueFamily& family);

        VertexBuffer(GeometryBufferAllocator& alloc,
                     IBuffer&                 buffer,
                     size_t                   vtxCount,
                     size_t                   vtxSize,
                     VmaVirtualAllocation     allocation,
                     size_t                   vtxOffset);

        VertexBuffer(const VertexBuffer&) = delete;

        VertexBuffer(VertexBuffer&&) = delete;

        ~VertexBuffer() noexcept override;

        VertexBuffer& operator=(const VertexBuffer&) = delete;

        VertexBuffer& operator=(VertexBuffer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] GeometryBufferAllocator& getAllocator() noexcept;

        [[nodiscard]] const GeometryBufferAllocator& getAllocator() const noexcept;

        /**
         * \brief Get the number of vertices in this vertex buffer.
         * \return Number of vertices.
         */
        [[nodiscard]] size_t getVertexCount() const noexcept;

        /**
         * \brief Get the size of each vertex.
         * \return Size in bytes.
         */
        [[nodiscard]] size_t getVertexSize() const noexcept;

        /**
         * \brief Get the offset into global buffer in number of vertices.
         * \return Offset in number of vertices.
         */
        [[nodiscard]] size_t getVertexOffset() const noexcept;

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
         * \brief Set the vertex data in the range [offset, offset + count).
         * \param transaction Transaction to append to.
         * \param data Pointer to data. Will be copied into a staging buffer. Can be released directly after this call.
         * \param count Number of vertices in data.
         * \param offset Offset into vertex buffer, in number of vertices.
         * \param barrier Barrier placed around the copy command. Transfer stage and access are automatically taken care of.
         * \param waitOnAllocFailure If true and staging buffer allocation fails, wait on previous transaction(s)
         * to complete so that they release their staging buffers, and try allocating again.
         * \return True if staging buffer allocation succeeded and transaction can be committed.
         * On failure, already staged transactions should be committed and waited on before trying again.
         */
        [[nodiscard]] bool setVertexData(Transaction&   transaction,
                                         const void*    data,
                                         size_t         count,
                                         size_t         offset,
                                         const Barrier& barrier,
                                         bool           waitOnAllocFailure);

        /**
         * \brief Get the vertex data in the range [srcOffset, srcOffset + count). Copies the data to a destination buffer.
         * Note: destination buffer is interpreted as a vertex buffer. Check the units of the count and offsets.
         * \param transaction Transaction to append to.
         * \param dstBuffer Destination buffer. Must be a vertex buffer with an equal vertex size.
         * \param srcBarrier Barrier placed around the copy command for the source buffer.
         * \param dstBarrier Barrier placed around the copy command for the destination buffer.
         * \param count Number of vertices to copy.
         * \param srcOffset Offset into the source buffer, in number of vertices.
         * \param dstOffset Offset into the destination buffer, in number of vertices.
         */
        void getVertexData(Transaction&   transaction,
                           VertexBuffer&  dstBuffer,
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
         * \brief Number of vertices.
         */
        size_t vertexCount = 0;

        /**
         * \brief Size of each vertex in bytes.
         */
        size_t vertexSize = 0;

        /**
         * \brief Allocation into global buffer.
         */
        VmaVirtualAllocation virtualAllocation = VK_NULL_HANDLE;

        /**
         * \brief Offset into global buffer in number of vertices.
         */
        size_t vertexOffset = 0;
    };
}  // namespace sol
