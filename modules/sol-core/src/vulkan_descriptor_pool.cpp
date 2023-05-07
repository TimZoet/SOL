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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanDescriptorPool::VulkanDescriptorPool(const Settings& set, const VkDescriptorPool vkPool) :
        settings(set), pool(vkPool)
    {
    }
#else
    VulkanDescriptorPool::VulkanDescriptorPool(const Settings& set, const VkDescriptorPool vkPool) :
        device(&set.device()), pool(vkPool)
    {
    }
#endif

    VulkanDescriptorPool::~VulkanDescriptorPool() noexcept
    {
        vkDestroyDescriptorPool(getDevice().get(), pool, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDescriptorPoolPtr VulkanDescriptorPool::create(const Settings& settings)
    {
        auto layout = createImpl(settings);
        return std::make_unique<VulkanDescriptorPool>(settings, layout);
    }

    VulkanDescriptorPoolSharedPtr VulkanDescriptorPool::createShared(const Settings& settings)
    {
        auto layout = createImpl(settings);
        return std::make_shared<VulkanDescriptorPool>(settings, layout);
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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanDescriptorPool::Settings& VulkanDescriptorPool::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanDescriptorPool::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanDescriptorPool::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkDescriptorPool& VulkanDescriptorPool::get() const noexcept { return pool; }
}  // namespace sol