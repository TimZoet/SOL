#include "sol-memory/transfer_manager.h"

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

#include "sol-memory/buffer_transaction.h"
#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TransferManager::TransferManager(MemoryManager& memoryManager, RingBufferMemoryPool& memoryPool) :
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

    TransferManager::~TransferManager() noexcept
    {
        // TODO: If there is a deadlock somewhere, this will get stuck. Wait with a timeout instead?
        static_cast<void>(waitAndLock());
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& TransferManager::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& TransferManager::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& TransferManager::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& TransferManager::getMemoryManager() const noexcept { return *manager; }

    RingBufferMemoryPool& TransferManager::getMemoryPool() const noexcept { return *pool; }

    ////////////////////////////////////////////////////////////////
    // Transactions.
    ////////////////////////////////////////////////////////////////

    BufferTransactionPtr TransferManager::beginTransaction() { return std::make_unique<BufferTransaction>(*this); }

    std::unique_ptr<std::scoped_lock<std::mutex>> TransferManager::waitAndLock()
    {
        auto l = lock();
        wait();
        return std::move(l);
    }

    void TransferManager::wait()
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
    }

    std::unique_ptr<std::scoped_lock<std::mutex>> TransferManager::lock()
    {
        return std::make_unique<std::scoped_lock<std::mutex>>(mutex);
    }

}  // namespace sol