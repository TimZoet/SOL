#include "sol-mesh/vertex_buffer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/geometry_buffer_allocator.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VertexBuffer::VertexBuffer(GeometryBufferAllocator& alloc,
                               VulkanBufferPtr          buffer,
                               const size_t             vtxCount,
                               const size_t             vtxSize,
                               const VulkanQueueFamily& family) :
        IBuffer(alloc.getMemoryManager(), family),
        allocator(&alloc),
        freeBuffer(std::move(buffer)),
        vertexCount(vtxCount),
        vertexSize(vtxSize)
    {
    }

    VertexBuffer::VertexBuffer(GeometryBufferAllocator&   alloc,
                               IBuffer&                   buffer,
                               const size_t               vtxCount,
                               const size_t               vtxSize,
                               const VmaVirtualAllocation allocation,
                               const size_t               vtxOffset) :
        IBuffer(alloc.getMemoryManager(), buffer.getQueueFamily()),
        allocator(&alloc),
        globalBuffer(&buffer),
        vertexCount(vtxCount),
        vertexSize(vtxSize),
        virtualAllocation(allocation),
        vertexOffset(vtxOffset)
    {
    }

    VertexBuffer::~VertexBuffer() noexcept
    {
        if (virtualAllocation) vmaVirtualFree(allocator->getVirtualVertexBlock(), virtualAllocation);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    GeometryBufferAllocator& VertexBuffer::getAllocator() noexcept { return *allocator; }

    const GeometryBufferAllocator& VertexBuffer::getAllocator() const noexcept { return *allocator; }

    size_t VertexBuffer::getVertexCount() const noexcept { return vertexCount; }

    size_t VertexBuffer::getVertexSize() const noexcept { return vertexSize; }

    size_t VertexBuffer::getVertexOffset() const noexcept { return vertexOffset; }

    VulkanBuffer& VertexBuffer::getBuffer() { return isSubAllocation() ? globalBuffer->getBuffer() : *freeBuffer; }

    const VulkanBuffer& VertexBuffer::getBuffer() const
    {
        return isSubAllocation() ? globalBuffer->getBuffer() : *freeBuffer;
    }

    size_t VertexBuffer::getBufferSize() const noexcept { return vertexSize * vertexCount; }

    size_t VertexBuffer::getBufferOffset() const noexcept { return isSubAllocation() ? vertexOffset * vertexSize : 0; }

    bool VertexBuffer::isSubAllocation() const noexcept { return globalBuffer != nullptr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////
    // Transactions.
    ////////////////////////////////////////////////////////////////

    bool VertexBuffer::setVertexData(Transaction&   transaction,
                                     const void*    data,
                                     const size_t   count,
                                     const size_t   offset,
                                     const Barrier& barrier,
                                     const bool     waitOnAllocFailure)
    {
        return setData(
          transaction, data, count * getVertexSize(), offset * getVertexSize(), barrier, waitOnAllocFailure);
    }

    void VertexBuffer::getVertexData(Transaction&   transaction,
                                     VertexBuffer&  dstBuffer,
                                     const Barrier& srcBarrier,
                                     const Barrier& dstBarrier,
                                     const size_t   count,
                                     const size_t   srcOffset,
                                     const size_t   dstOffset)
    {
        if (getVertexSize() != dstBuffer.getVertexSize())
            throw SolError("Cannot copy vertices to other vertex buffer. Vertex sizes do not match.");

        // Convert vertex counts to bytes.
        getData(transaction,
                dstBuffer,
                srcBarrier,
                dstBarrier,
                count * getVertexSize(),
                srcOffset * getVertexSize(),
                dstOffset * getVertexSize());
    }
}  // namespace sol
