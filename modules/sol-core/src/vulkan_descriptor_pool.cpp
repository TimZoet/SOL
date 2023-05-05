#include "sol-core/vulkan_descriptor_pool.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanDescriptorPool::VulkanDescriptorPool(SettingsPtr settingsPtr, const VkDescriptorPool vkPool) :
        settings(std::move(settingsPtr)), pool(vkPool)
    {
    }

    VulkanDescriptorPool::~VulkanDescriptorPool() noexcept { vkDestroyDescriptorPool(settings->device, pool, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDescriptorPoolPtr VulkanDescriptorPool::create(Settings settings)
    {
        auto layout = createImpl(settings);
        return std::make_unique<VulkanDescriptorPool>(std::make_unique<Settings>(std::move(settings)), layout);
    }

    VulkanDescriptorPoolSharedPtr VulkanDescriptorPool::createShared(Settings settings)
    {
        auto layout = createImpl(settings);
        return std::make_shared<VulkanDescriptorPool>(std::make_unique<Settings>(std::move(settings)), layout);
    }

    VkDescriptorPool VulkanDescriptorPool::createImpl(const Settings& settings)
    {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets       = settings.maxSets;
        poolInfo.poolSizeCount = static_cast<uint32_t>(settings.poolSizes.size());
        poolInfo.pPoolSizes    = settings.poolSizes.data();

        // Create pool.
        VkDescriptorPool pool;
        handleVulkanError(vkCreateDescriptorPool(settings.device, &poolInfo, nullptr, &pool));

        return pool;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanDescriptorPool::Settings& VulkanDescriptorPool::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanDescriptorPool::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanDescriptorPool::getDevice() const noexcept { return settings->device(); }

    const VkDescriptorPool& VulkanDescriptorPool::get() const noexcept { return pool; }
}  // namespace sol