#include "sol-core/vulkan_command_pool.h"

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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanCommandPool::VulkanCommandPool(const Settings& set, const VkCommandPool vkCommandPool) :
        settings(set), commandPool(vkCommandPool)
    {
    }
#else
    VulkanCommandPool::VulkanCommandPool(const Settings& set, const VkCommandPool vkCommandPool) :
        device(&set.device()), flags(set.flags), commandPool(vkCommandPool)
    {
    }
#endif

    VulkanCommandPool::~VulkanCommandPool() noexcept { vkDestroyCommandPool(getDevice().get(), commandPool, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanCommandPoolPtr VulkanCommandPool::create(const Settings& settings)
    {
        auto commandPool = createImpl(settings);
        return std::make_unique<VulkanCommandPool>(settings, commandPool);
    }

    VulkanCommandPoolSharedPtr VulkanCommandPool::createShared(const Settings& settings)
    {
        auto commandPool = createImpl(settings);
        return std::make_shared<VulkanCommandPool>(settings, commandPool);
    }

    VkCommandPool VulkanCommandPool::createImpl(const Settings& settings)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = settings.queueFamilyIndex;
        poolInfo.flags            = settings.flags;

        // Create command pool.
        VkCommandPool commandPool;
        handleVulkanError(vkCreateCommandPool(settings.device, &poolInfo, nullptr, &commandPool));

        return commandPool;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanCommandPool::Settings& VulkanCommandPool::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanCommandPool::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanCommandPool::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkCommandPool& VulkanCommandPool::get() const noexcept { return commandPool; }

    bool VulkanCommandPool::isResettable() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.flags & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
#else
        return flags & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
#endif
    }

    ////////////////////////////////////////////////////////////////
    // Command buffers.
    ////////////////////////////////////////////////////////////////

    VulkanCommandBufferPtr VulkanCommandPool::createCommandBuffer(const VkCommandBufferLevel level)
    {
        VulkanCommandBuffer::Settings bufferSettings;
        bufferSettings.commandPool = *this;
        bufferSettings.level       = level;
        return VulkanCommandBuffer::create(bufferSettings);
    }

}  // namespace sol