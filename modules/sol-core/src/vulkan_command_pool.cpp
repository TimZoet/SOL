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

    VulkanCommandPool::VulkanCommandPool(SettingsPtr settingsPtr, const VkCommandPool vkCommandPool) :
        settings(std::move(settingsPtr)), commandPool(vkCommandPool)
    {
    }

    VulkanCommandPool::~VulkanCommandPool() noexcept { vkDestroyCommandPool(settings->device, commandPool, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanCommandPoolPtr VulkanCommandPool::create(Settings settings)
    {
        auto commandPool = createImpl(settings);
        return std::make_unique<VulkanCommandPool>(std::make_unique<Settings>(settings), commandPool);
    }

    VulkanCommandPoolSharedPtr VulkanCommandPool::createShared(Settings settings)
    {
        auto commandPool = createImpl(settings);
        return std::make_shared<VulkanCommandPool>(std::make_unique<Settings>(settings), commandPool);
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

    const VulkanCommandPool::Settings& VulkanCommandPool::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanCommandPool::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanCommandPool::getDevice() const noexcept { return settings->device(); }

    const VkCommandPool& VulkanCommandPool::get() const noexcept { return commandPool; }

    bool VulkanCommandPool::isResettable() const noexcept
    {
        return settings->flags & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
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