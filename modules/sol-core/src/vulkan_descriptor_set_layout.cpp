#include "sol-core/vulkan_descriptor_set_layout.h"

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
    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const Settings& set, const VkDescriptorSetLayout vkLayout) :
        settings(set), layout(vkLayout)
    {
    }
#else
    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const Settings& set, const VkDescriptorSetLayout vkLayout) :
        device(&set.device()), layout(vkLayout)
    {
    }
#endif

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() noexcept
    {
        vkDestroyDescriptorSetLayout(getDevice().get(), layout, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDescriptorSetLayoutPtr VulkanDescriptorSetLayout::create(const Settings& settings)
    {
        auto layout = createImpl(settings);
        return std::make_unique<VulkanDescriptorSetLayout>(settings, layout);
    }

    VulkanDescriptorSetLayoutSharedPtr VulkanDescriptorSetLayout::createShared(const Settings& settings)
    {
        auto layout = createImpl(settings);
        return std::make_shared<VulkanDescriptorSetLayout>(settings, layout);
    }

    VkDescriptorSetLayout VulkanDescriptorSetLayout::createImpl(const Settings& settings)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.flags        = settings.flags;
        layoutInfo.bindingCount = static_cast<uint32_t>(settings.bindings.size());
        layoutInfo.pBindings    = settings.bindings.data();

        // Create layout.
        VkDescriptorSetLayout layout;
        handleVulkanError(vkCreateDescriptorSetLayout(settings.device, &layoutInfo, nullptr, &layout));

        return layout;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanDescriptorSetLayout::Settings& VulkanDescriptorSetLayout::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanDescriptorSetLayout::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanDescriptorSetLayout::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkDescriptorSetLayout& VulkanDescriptorSetLayout::get() const noexcept { return layout; }
}  // namespace sol