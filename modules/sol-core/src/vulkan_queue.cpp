#include "sol-core/vulkan_queue.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"


namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanQueue::VulkanQueue(VulkanDevice& dev, VulkanQueueFamily& qFamily, const VkQueue vkQueue) :
        device(&dev), family(&qFamily), queue(vkQueue)
    {
    }

    VulkanQueue::~VulkanQueue() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& VulkanQueue::getDevice() noexcept { return *device; }

    const VulkanDevice& VulkanQueue::getDevice() const noexcept { return *device; }

    const VulkanQueueFamily& VulkanQueue::getFamily() const noexcept { return *family; }

    const VkQueue& VulkanQueue::get() const noexcept { return queue; }

    bool VulkanQueue::isThreadSafe() const noexcept { return threadSafe; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void VulkanQueue::setThreadSafe(const bool safe) noexcept { threadSafe = safe; }

    ////////////////////////////////////////////////////////////////
    // Queue operations.
    ////////////////////////////////////////////////////////////////

    void VulkanQueue::submit(const VulkanCommandBuffer& commandBuffer)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer.get();

        submit(submitInfo);
    }

    void VulkanQueue::submit(const VulkanCommandBuffer* commandBuffers, const uint32_t count)
    {
        std::vector<VkCommandBuffer> handles(count, VK_NULL_HANDLE);
        for (uint32_t i = 0; i < count; i++) handles[i] = commandBuffers[i].get();

        VkSubmitInfo submitInfo{};
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = count;
        submitInfo.pCommandBuffers    = handles.data();

        submit(submitInfo);
    }

    void VulkanQueue::submit(const VkSubmitInfo& info) { submit(info, VK_NULL_HANDLE); }

    void VulkanQueue::submit(const VkSubmitInfo& info, const VkFence fence) { submit(&info, 1, fence); }

    void VulkanQueue::submit(const VkSubmitInfo* infos, const uint32_t count) { submit(infos, count, VK_NULL_HANDLE); }

    void VulkanQueue::submit(const VkSubmitInfo* infos, const uint32_t count, const VkFence fence)
    {
        std::unique_lock lock(mutex, std::defer_lock);
        if (threadSafe) lock.lock();

        handleVulkanError(vkQueueSubmit(queue, count, infos, fence));
    }

    void VulkanQueue::waitIdle()
    {
        std::unique_lock lock(mutex, std::defer_lock);
        if (threadSafe) lock.lock();

        handleVulkanError(vkQueueWaitIdle(queue));
    }
}  // namespace sol
