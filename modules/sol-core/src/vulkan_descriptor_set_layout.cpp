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

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(SettingsPtr                 settingsPtr,
                                                         const VkDescriptorSetLayout vkLayout) :
        settings(std::move(settingsPtr)), layout(vkLayout)
    {
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() noexcept
    {
        vkDestroyDescriptorSetLayout(settings->device, layout, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDescriptorSetLayoutPtr VulkanDescriptorSetLayout::create(Settings settings)
    {
        auto layout = createImpl(settings);
        return std::make_unique<VulkanDescriptorSetLayout>(std::make_unique<Settings>(std::move(settings)), layout);
    }

    VulkanDescriptorSetLayoutSharedPtr VulkanDescriptorSetLayout::createShared(Settings settings)
    {
        auto layout = createImpl(settings);
        return std::make_shared<VulkanDescriptorSetLayout>(std::make_unique<Settings>(std::move(settings)), layout);
    }

    VkDescriptorSetLayout VulkanDescriptorSetLayout::createImpl(const Settings& settings)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.flags        = 0;
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

    const VulkanDescriptorSetLayout::Settings& VulkanDescriptorSetLayout::getSettings() const noexcept
    {
        return *settings;
    }

    VulkanDevice& VulkanDescriptorSetLayout::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanDescriptorSetLayout::getDevice() const noexcept { return settings->device(); }

    const VkDescriptorSetLayout& VulkanDescriptorSetLayout::get() const noexcept { return layout; }
}  // namespace sol