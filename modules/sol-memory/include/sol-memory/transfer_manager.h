#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <mutex>
#include <queue>

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
#include "sol-memory/pool/ring_buffer_memory_pool.h"

namespace sol
{
    class TransferManager
    {
    public:
        friend class BufferTransaction;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TransferManager() = delete;

        TransferManager(MemoryManager& memoryManager, RingBufferMemoryPool& memoryPool);

        TransferManager(const TransferManager&) = delete;

        TransferManager(TransferManager&&) noexcept = delete;

        virtual ~TransferManager() noexcept;

        TransferManager& operator=(const TransferManager&) = delete;

        TransferManager& operator=(TransferManager&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        [[nodiscard]] RingBufferMemoryPool& getMemoryPool() const noexcept;

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
        [[nodiscard]] std::unique_ptr<std::scoped_lock<std::mutex>> waitAndLock();

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
    };
}  // namespace sol
