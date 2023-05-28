#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MemoryManager::MemoryManager(VulkanDevice& vkDevice) : device(&vkDevice)
    {
        initializeAllocator();
        initializeCommandPools();
    }

    MemoryManager::MemoryManager(VulkanMemoryAllocatorPtr alloc) :
        device(&alloc->getDevice()), allocator(std::move(alloc))
    {
        initializeCommandPools();
    }

    MemoryManager::~MemoryManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& MemoryManager::getDevice() noexcept { return *device; }

    const VulkanDevice& MemoryManager::getDevice() const noexcept { return *device; }

    VulkanMemoryAllocator& MemoryManager::getAllocator() noexcept { return *allocator; }

    const VulkanMemoryAllocator& MemoryManager::getAllocator() const noexcept { return *allocator; }

    VulkanCommandPool& MemoryManager::getCommandPool(const VulkanQueueFamily& queueFamily) const
    {
        return getCommandPool(queueFamily.getIndex());
    }

    VulkanCommandPool& MemoryManager::getCommandPool(const uint32_t queueFamilyIndex) const
    {
        if (queueFamilyIndex > commandPools.size())
            throw SolError("Cannot retrieve command pool. Queue family index out of range.");
        return *commandPools.at(queueFamilyIndex);
    }

    VulkanQueue& MemoryManager::getQueue(const VulkanQueueFamily& queueFamily) const
    {
        return getQueue(queueFamily.getIndex());
    }

    VulkanQueue& MemoryManager::getQueue(const uint32_t queueFamilyIndex) const
    {
        if (computeQueue && computeQueue->getFamily().getIndex() == queueFamilyIndex) return *computeQueue;
        if (graphicsQueue && graphicsQueue->getFamily().getIndex() == queueFamilyIndex) return *graphicsQueue;
        if (transferQueue && transferQueue->getFamily().getIndex() == queueFamilyIndex) return *transferQueue;
        throw SolError("");
    }

    VulkanQueue& MemoryManager::getComputeQueue() const
    {
        if (!computeQueue) throw SolError("");
        return *computeQueue;
    }

    VulkanQueue& MemoryManager::getGraphicsQueue() const
    {
        if (!graphicsQueue) throw SolError("");
        return *graphicsQueue;
    }

    VulkanQueue& MemoryManager::getTransferQueue() const
    {
        if (!transferQueue) return getGraphicsQueue();
        return *transferQueue;
    }

    ////////////////////////////////////////////////////////////////
    // Initialization.
    ////////////////////////////////////////////////////////////////

    void MemoryManager::initializeAllocator()
    {
        VulkanMemoryAllocator::Settings settings;
        settings.device = *device;
        allocator       = VulkanMemoryAllocator::create(settings);
    }

    void MemoryManager::initializeCommandPools()
    {
        // Create a resettable command pool for each queue family.
        const auto size = device->getPhysicalDevice().getQueueFamilies().size();
        commandPools.reserve(size);
        for (uint32_t i = 0; i < size; i++)
        {
            VulkanCommandPool::Settings commandPoolSettings;
            commandPoolSettings.device           = *device;
            commandPoolSettings.queueFamilyIndex = i;
            commandPoolSettings.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPools.emplace_back(VulkanCommandPool::create(commandPoolSettings));
        }
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MemoryManager::setComputeQueue(VulkanQueue& queue)
    {
        if (&queue.getDevice() != device) throw SolError("Cannot set compute queue. Queue is on a different device.");
        if (!queue.getFamily().supportsCompute())
            throw SolError("Cannot set compute queue. It does not support compute operations.");

        computeQueue = &queue;
    }

    void MemoryManager::setGraphicsQueue(VulkanQueue& queue)
    {
        if (&queue.getDevice() != device) throw SolError("Cannot set graphics queue. Queue is on a different device.");
        if (!queue.getFamily().supportsGraphics())
            throw SolError("Cannot set graphics queue. It does not support graphics operations.");

        graphicsQueue = &queue;
    }

    void MemoryManager::setTransferQueue(VulkanQueue& queue)
    {
        if (&queue.getDevice() != device) throw SolError("Cannot set transfer queue. Queue is on a different device.");
        if (!queue.getFamily().supportsTransfer())
            throw SolError("Cannot set transfer queue. It does not support transfer operations.");

        transferQueue = &queue;
    }
}  // namespace sol