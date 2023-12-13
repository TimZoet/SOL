#include "sol-mesh/index_buffer.h"

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

    IndexBuffer::IndexBuffer(GeometryBufferAllocator& alloc,
                             VulkanBufferPtr          buffer,
                             const size_t             idxCount,
                             const size_t             idxSize,
                             const VulkanQueueFamily& family) :
        IBuffer(alloc.getMemoryManager(), family),
        allocator(&alloc),
        freeBuffer(std::move(buffer)),
        indexCount(idxCount),
        indexSize(idxSize)
    {
    }

    IndexBuffer::IndexBuffer(GeometryBufferAllocator&   alloc,
                             IBuffer&                   buffer,
                             const size_t               idxCount,
                             const size_t               idxSize,
                             const VmaVirtualAllocation allocation,
                             const size_t               idxOffset) :
        IBuffer(alloc.getMemoryManager(), buffer.getQueueFamily()),
        allocator(&alloc),
        globalBuffer(&buffer),
        indexCount(idxCount),
        indexSize(idxSize),
        virtualAllocation(allocation),
        indexOffset(idxOffset)
    {
    }

    IndexBuffer::~IndexBuffer() noexcept
    {
        if (virtualAllocation) vmaVirtualFree(allocator->getVirtualIndexBlock(), virtualAllocation);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    GeometryBufferAllocator& IndexBuffer::getAllocator() noexcept { return *allocator; }

    const GeometryBufferAllocator& IndexBuffer::getAllocator() const noexcept { return *allocator; }

    size_t IndexBuffer::getIndexCount() const noexcept { return indexCount; }

    size_t IndexBuffer::getIndexSize() const noexcept { return indexSize; }

    VulkanBuffer& IndexBuffer::getBuffer() { return isSubAllocation() ? globalBuffer->getBuffer() : *freeBuffer; }

    const VulkanBuffer& IndexBuffer::getBuffer() const
    {
        return isSubAllocation() ? globalBuffer->getBuffer() : *freeBuffer;
    }

    size_t IndexBuffer::getBufferSize() const noexcept { return indexSize * indexCount; }

    size_t IndexBuffer::getBufferOffset() const noexcept { return isSubAllocation() ? indexOffset * indexSize : 0; }

    bool IndexBuffer::isSubAllocation() const noexcept { return globalBuffer != nullptr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////
    // Transactions.
    ////////////////////////////////////////////////////////////////

    bool IndexBuffer::setIndexData(Transaction&   transaction,
                                   const void*    data,
                                   const size_t   count,
                                   const size_t   offset,
                                   const Barrier& barrier,
                                   const bool     waitOnAllocFailure)
    {
        return setData(transaction, data, count * getIndexSize(), offset * getIndexSize(), barrier, waitOnAllocFailure);
    }

    void IndexBuffer::getIndexData(Transaction&   transaction,
                                   IndexBuffer&   dstBuffer,
                                   const Barrier& srcBarrier,
                                   const Barrier& dstBarrier,
                                   const size_t   count,
                                   const size_t   srcOffset,
                                   const size_t   dstOffset)
    {
        if (getIndexSize() != dstBuffer.getIndexSize())
            throw SolError("Cannot copy indices to other index buffer. Index sizes do not match.");

        // Convert index counts to bytes.
        getData(transaction,
                dstBuffer,
                srcBarrier,
                dstBarrier,
                count * getIndexSize(),
                srcOffset * getIndexSize(),
                dstOffset * getIndexSize());
    }
}  // namespace sol
