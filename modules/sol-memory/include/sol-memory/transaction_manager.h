#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <mutex>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/fwd.h"
#include "sol-memory/pool/ring_buffer_memory_pool.h"

namespace sol
{
    class TransactionManager
    {
    public:
        friend class Transaction;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TransactionManager() = delete;

        TransactionManager(MemoryManager& memoryManager, RingBufferMemoryPool& memoryPool);

        TransactionManager(const TransactionManager&) = delete;

        TransactionManager(TransactionManager&&) noexcept = delete;

        virtual ~TransactionManager() noexcept;

        TransactionManager& operator=(const TransactionManager&) = delete;

        TransactionManager& operator=(TransactionManager&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] static TransactionManagerPtr create(MemoryManager& memoryManager, size_t memoryPoolSize);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        [[nodiscard]] RingBufferMemoryPool& getMemoryPool() const noexcept;

        [[nodiscard]] const std::vector<VulkanTimelineSemaphorePtr>& getSemaphores() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Transactions.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] BufferTransactionPtr beginTransaction();

    private:
        /**
         * \brief Wait for currently active transaction to complete and then return a lock.
         * While holding the lock, the command buffers and semaphores can be used.
         * \return Lock.
         */
        [[nodiscard]] std::unique_ptr<std::scoped_lock<std::mutex>> lockAndWait();

        [[nodiscard]] std::unique_ptr<std::scoped_lock<std::mutex>> lock();

        void wait();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* manager = nullptr;

        RingBufferMemoryPool* pool = nullptr;

        std::mutex mutex;

        std::vector<VulkanCommandBufferPtr>     preCopyReleaseCmdBuffers;
        std::vector<VulkanCommandBufferPtr>     preCopyAcquireCmdBuffers;
        std::vector<VulkanCommandBufferPtr>     postCopyReleaseCmdBuffers;
        std::vector<VulkanCommandBufferPtr>     postCopyAcquireCmdBuffers;
        VulkanCommandBufferPtr                  copyCmdBuffer;
        std::vector<VulkanTimelineSemaphorePtr> semaphores;
        std::vector<uint64_t>                   semaphoreValues;
        size_t                                  transactionIndex = 0;
        std::vector<IBufferPtr>                 pendingStagingBuffers;
    };
}  // namespace sol
