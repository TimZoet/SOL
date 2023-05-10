#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

namespace sol
{
    class MemoryManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MemoryManager() = default;

        explicit MemoryManager(VulkanDevice& vkDevice);

        MemoryManager(const MemoryManager&) = delete;

        MemoryManager(MemoryManager&&) = delete;

        ~MemoryManager() noexcept;

        MemoryManager& operator=(const MemoryManager&) = delete;

        MemoryManager& operator=(MemoryManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] VulkanMemoryAllocator& getAllocator() noexcept;

        [[nodiscard]] const VulkanMemoryAllocator& getAllocator() const noexcept;

        [[nodiscard]] VulkanCommandPool& getCommandPool(const VulkanQueueFamily& queueFamily) const;

        [[nodiscard]] VulkanCommandPool& getCommandPool(uint32_t queueFamilyIndex) const;

        [[nodiscard]] VulkanQueue& getQueue(const VulkanQueueFamily& queueFamily) const;

        [[nodiscard]] VulkanQueue& getQueue(uint32_t queueFamilyIndex) const;

        [[nodiscard]] VulkanQueue& getComputeQueue() const;

        [[nodiscard]] VulkanQueue& getGraphicsQueue() const;

        [[nodiscard]] VulkanQueue& getTransferQueue() const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setComputeQueue(VulkanQueue& queue);

        void setGraphicsQueue(VulkanQueue& queue);

        void setTransferQueue(VulkanQueue& queue);

    private:
        ////////////////////////////////////////////////////////////////
        // Initialization.
        ////////////////////////////////////////////////////////////////

        void initializeAllocator();

        void initializeCommandPools();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        VulkanMemoryAllocatorPtr allocator;

        VulkanQueue* computeQueue = nullptr;

        VulkanQueue* graphicsQueue = nullptr;

        VulkanQueue* transferQueue = nullptr;

        std::vector<VulkanCommandPoolPtr> commandPools;
    };
}  // namespace sol
