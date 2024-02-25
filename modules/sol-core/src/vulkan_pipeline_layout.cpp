#include "sol-core/vulkan_pipeline_layout.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanPipelineLayout::VulkanPipelineLayout(Settings set,

                                               const VkPipelineLayout vkLayout) :
        settings(std::move(set)), pipelineLayout(vkLayout)
    {
    }

    VulkanPipelineLayout::~VulkanPipelineLayout() noexcept
    {
        vkDestroyPipelineLayout(getDevice().get(), pipelineLayout, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanPipelineLayoutPtr VulkanPipelineLayout::create(const Settings& settings)
    {
        auto layout = createImpl(settings);
        return std::make_unique<VulkanPipelineLayout>(settings, layout);
    }

    VulkanPipelineLayoutSharedPtr VulkanPipelineLayout::createShared(const Settings& settings)
    {
        auto layout = createImpl(settings);
        return std::make_shared<VulkanPipelineLayout>(settings, layout);
    }

    VkPipelineLayout VulkanPipelineLayout::createImpl(const Settings& settings)
    {
        const auto setLayouts = settings.descriptors.get();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.flags =
          settings.independentDescriptors ? VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT : 0;
        pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts            = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(settings.pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges    = settings.pushConstants.data();

        VkPipelineLayout layout;
        handleVulkanError(vkCreatePipelineLayout(settings.device, &pipelineLayoutInfo, nullptr, &layout));

        return layout;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanPipelineLayout::Settings& VulkanPipelineLayout::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanPipelineLayout::getDevice() noexcept { return settings.device(); }

    const VulkanDevice& VulkanPipelineLayout::getDevice() const noexcept { return settings.device(); }

    const VkPipelineLayout& VulkanPipelineLayout::get() const noexcept { return pipelineLayout; }
}  // namespace sol
