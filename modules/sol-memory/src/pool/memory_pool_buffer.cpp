#include "sol-memory/pool/memory_pool_buffer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/pool/i_memory_pool.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MemoryPoolBuffer::MemoryPoolBuffer(IMemoryPool&       memoryPool,
                                       VulkanQueueFamily& queueFamily,
                                       const size_t       id,
                                       VulkanBuffer&      buffer,
                                       const size_t       bufferSize,
                                       const size_t       bufferOffset) :
        IBuffer(memoryPool.getMemoryManager(), queueFamily),
        pool(&memoryPool),
        identifier(id),
        buffer(&buffer),
        size(bufferSize),
        offset(bufferOffset)
    {
    }

    MemoryPoolBuffer::MemoryPoolBuffer(MemoryPoolBuffer&&) noexcept = default;

    MemoryPoolBuffer::~MemoryPoolBuffer() noexcept { pool->releaseBuffer(*this); }

    MemoryPoolBuffer& MemoryPoolBuffer::operator=(MemoryPoolBuffer&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMemoryPool& MemoryPoolBuffer::getMemoryPool() noexcept { return *pool; }

    const IMemoryPool& MemoryPoolBuffer::getMemoryPool() const noexcept { return *pool; }

    size_t MemoryPoolBuffer::getId() const noexcept { return identifier; }

    VulkanBuffer& MemoryPoolBuffer::getBuffer() noexcept { return *buffer; }

    const VulkanBuffer& MemoryPoolBuffer::getBuffer() const noexcept { return *buffer; }

    size_t MemoryPoolBuffer::getBufferSize() const noexcept { return size; }

    size_t MemoryPoolBuffer::getBufferOffset() const noexcept { return offset; }

    bool MemoryPoolBuffer::isSubAllocation() const noexcept { return offset > 0; }

}  // namespace sol
