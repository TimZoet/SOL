#include "sol-memory/i_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IBuffer::IBuffer(MemoryManager& memoryManager, const VulkanQueueFamily& family) :
        manager(&memoryManager), queueFamily(&family)
    {
    }

    IBuffer::~IBuffer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& IBuffer::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& IBuffer::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& IBuffer::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& IBuffer::getMemoryManager() const noexcept { return *manager; }

    const VulkanQueueFamily& IBuffer::getQueueFamily() const noexcept { return *queueFamily; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void IBuffer::setQueueFamily(const VulkanQueueFamily& family) noexcept { queueFamily = &family; }

    ////////////////////////////////////////////////////////////////
    // Transactions.
    ////////////////////////////////////////////////////////////////

    void IBuffer::barrier(Transaction& transaction, const Barrier& barrier, const BarrierLocation location)
    {
        const BufferBarrier bufferBarrier{.buffer    = *this,
                                          .srcFamily = &getQueueFamily(),
                                          .dstFamily = barrier.dstFamily ? barrier.dstFamily : &getQueueFamily(),
                                          .srcStage  = barrier.srcStage,
                                          .dstStage  = barrier.dstStage,
                                          .srcAccess = barrier.srcAccess,
                                          .dstAccess = barrier.dstAccess};

        transaction.stage(bufferBarrier, location);
    }

    bool IBuffer::setData(Transaction&   transaction,
                          const void*    data,
                          const size_t   size,
                          const size_t   offset,
                          const Barrier& barrier,
                          const bool     waitOnAllocFailure)
    {
        const StagingBufferCopy copy{
          .dstBuffer = *this, .data = data, .size = size, .offset = offset, .dstOnDedicatedTransfer = true};

        // We just create a barrier for the entire buffer.
        const BufferBarrier bufferBarrier{.buffer    = *this,
                                          .srcFamily = &getQueueFamily(),
                                          .dstFamily = barrier.dstFamily ? barrier.dstFamily : &getQueueFamily(),
                                          .srcStage  = barrier.srcStage,
                                          .dstStage  = barrier.dstStage,
                                          .srcAccess = barrier.srcAccess,
                                          .dstAccess = barrier.dstAccess};

        return transaction.stage(copy, bufferBarrier, waitOnAllocFailure);
    }

    void IBuffer::getData(Transaction&   transaction,
                          IBuffer&       dstBuffer,
                          const Barrier& srcBarrier,
                          const Barrier& dstBarrier,
                          const size_t   size,
                          const size_t   srcOffset,
                          const size_t   dstOffset)
    {
        const BufferBarrier sbarrier{.buffer    = *this,
                                     .srcFamily = &getQueueFamily(),
                                     .dstFamily = srcBarrier.dstFamily ? srcBarrier.dstFamily : &getQueueFamily(),
                                     .srcStage  = srcBarrier.srcStage,
                                     .dstStage  = srcBarrier.dstStage,
                                     .srcAccess = srcBarrier.srcAccess,
                                     .dstAccess = srcBarrier.dstAccess};

        const BufferBarrier dbarrier{.buffer    = *this,
                                     .srcFamily = &dstBuffer.getQueueFamily(),
                                     .dstFamily = dstBarrier.dstFamily ? dstBarrier.dstFamily : &getQueueFamily(),
                                     .srcStage  = dstBarrier.srcStage,
                                     .dstStage  = dstBarrier.dstStage,
                                     .srcAccess = dstBarrier.srcAccess,
                                     .dstAccess = dstBarrier.dstAccess};

        const BufferToBufferCopy copy{.srcBuffer              = *this,
                                      .dstBuffer              = dstBuffer,
                                      .size                   = size,
                                      .srcOffset              = srcOffset,
                                      .dstOffset              = dstOffset,
                                      .srcOnDedicatedTransfer = true,
                                      .dstOnDedicatedTransfer = true };

        transaction.stage(copy, sbarrier, dbarrier);
    }

}  // namespace sol
