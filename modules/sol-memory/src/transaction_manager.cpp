#include "sol-memory/transaction_manager.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_timeline_semaphore.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/i_buffer.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TransactionManager::TransactionManager(MemoryManager& memoryManager, RingBufferMemoryPool& memoryPool) :
        manager(&memoryManager), pool(&memoryPool)
    {
        const auto familyCount = static_cast<uint32_t>(getDevice().getPhysicalDevice().getQueueFamilies().size());

        VulkanCommandBuffer::Settings cmdSettings;
        cmdSettings.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        VulkanTimelineSemaphore::Settings semSettings;
        semSettings.device = manager->getDevice();

        for (uint32_t i = 0; i < familyCount; i++)
        {
            cmdSettings.commandPool = manager->getCommandPool(i);
            preCopyReleaseCmdBuffers.emplace_back(VulkanCommandBuffer::create(cmdSettings));
            preCopyAcquireCmdBuffers.emplace_back(VulkanCommandBuffer::create(cmdSettings));
            postCopyReleaseCmdBuffers.emplace_back(VulkanCommandBuffer::create(cmdSettings));
            postCopyAcquireCmdBuffers.emplace_back(VulkanCommandBuffer::create(cmdSettings));

            if (manager->getTransferQueue().getFamily().getIndex() == i)
                copyCmdBuffer = VulkanCommandBuffer::create(cmdSettings);

            semaphores.emplace_back(VulkanTimelineSemaphore::create(semSettings));
            semaphoreValues.emplace_back(0);
        }
    }

    TransactionManager::~TransactionManager() noexcept
    {
        // TODO: If there is a deadlock somewhere, this will get stuck. Wait with a timeout instead?
        static_cast<void>(lockAndWait());
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    TransactionManagerPtr TransactionManager::create(MemoryManager& memoryManager, const size_t memoryPoolSize)
    {
        const IMemoryPool::CreateInfo info{
          .createFlags          = 0,
          .bufferUsage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
          .blockSize       = memoryPoolSize,
          .minBlocks       = 1,
          .maxBlocks       = 1};
        auto& pool = memoryManager.createRingBufferMemoryPool("transfer", info);
        return std::make_unique<TransactionManager>(memoryManager, pool);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& TransactionManager::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& TransactionManager::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& TransactionManager::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& TransactionManager::getMemoryManager() const noexcept { return *manager; }

    RingBufferMemoryPool& TransactionManager::getMemoryPool() const noexcept { return *pool; }

    const std::vector<VulkanTimelineSemaphorePtr>& TransactionManager::getSemaphores() const noexcept
    {
        return semaphores;
    }

    ////////////////////////////////////////////////////////////////
    // Transactions.
    ////////////////////////////////////////////////////////////////

    BufferTransactionPtr TransactionManager::beginTransaction() { return std::make_unique<Transaction>(*this); }

    std::unique_ptr<std::scoped_lock<std::mutex>> TransactionManager::lockAndWait()
    {
        auto l = lock();
        wait();
        return std::move(l);
    }

    void TransactionManager::wait()
    {
        const auto handles =
          semaphores | std::views::transform([](const auto& s) { return s->get(); }) | std::ranges::to<std::vector>();

        const VkSemaphoreWaitInfo info{.sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                                       .pNext          = nullptr,
                                       .flags          = 0,
                                       .semaphoreCount = static_cast<uint32_t>(handles.size()),
                                       .pSemaphores    = handles.data(),
                                       .pValues        = semaphoreValues.data()};

        handleVulkanError(vkWaitSemaphores(getDevice().get(), &info, UINT64_MAX));
        pendingStagingBuffers.clear();
    }

    std::unique_ptr<std::scoped_lock<std::mutex>> TransactionManager::lock()
    {
        return std::make_unique<std::scoped_lock<std::mutex>>(mutex);
    }

}  // namespace sol
