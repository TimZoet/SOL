#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <atomic>
#include <mutex>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

namespace sol
{
    class VulkanQueue
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanQueue() = delete;

        VulkanQueue(VulkanDevice& dev, VulkanQueueFamily& qFamily, VkQueue vkQueue);

        VulkanQueue(const VulkanQueue&) = delete;

        VulkanQueue(VulkanQueue&&) = delete;

        ~VulkanQueue() noexcept;

        VulkanQueue& operator=(const VulkanQueue&) = delete;

        VulkanQueue& operator=(VulkanQueue&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        /**
         * \brief Get the queue family.
         * \return Queue family.
         */
        [[nodiscard]] const VulkanQueueFamily& getFamily() const noexcept;

        /**
         * \brief Get the queue managed by this object.
         * \return Queue handle.
         */
        [[nodiscard]] const VkQueue& get() const noexcept;

        /**
         * \brief Returns whether submits to this queue are synchronized internally.
         * \return True if thread safe, false otherwise.
         */
        [[nodiscard]] bool isThreadSafe() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Enable or disable thread safe submits.
         * \param safe True to make thread safe.
         */
        void setThreadSafe(bool safe) noexcept;

        ////////////////////////////////////////////////////////////////
        // Queue operations.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Submit a single command buffer to this queue.
         * \param commandBuffer Command buffer.
         */
        void submit(const VulkanCommandBuffer& commandBuffer);

        /**
         * \brief Submit multiple command buffers to this queue at once.
         * \param commandBuffers List of count VulkanCommandBuffers. Should not be null and contain only valid objects.
         * \param count Number of command buffers.
         */
        void submit(const VulkanCommandBuffer* commandBuffers, uint32_t count);

        void submit(const VkSubmitInfo& info);

        void submit(const VkSubmitInfo& info, VkFence fence);

        void submit(const VkSubmitInfo* infos, uint32_t count);

        void submit(const VkSubmitInfo* infos, uint32_t count, VkFence fence);

        /**
         * \brief Wait until queue is idle. Internally calls vkQueueWaitIdle.
         */
        void waitIdle();

    private:
        VulkanDevice* device = nullptr;

        /**
         * \brief Queue family.
         */
        VulkanQueueFamily* family = nullptr;

        /**
         * \brief Vulkan queue.
         */
        VkQueue queue = VK_NULL_HANDLE;

        /**
         * \brief Mutex for thread safe queue.
         */
        std::mutex mutex;

        /**
         * \brief Whether thread safety is enabled for this queue.
         */
        std::atomic_bool threadSafe = false;
    };
}  // namespace sol