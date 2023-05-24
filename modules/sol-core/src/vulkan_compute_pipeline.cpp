#include "sol-core/vulkan_compute_pipeline.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_shader_module.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanComputePipeline::VulkanComputePipeline(Settings               set,
                                                 const VkPipeline       vkPipeline,
                                                 const VkPipelineLayout vkLayout) :
        settings(std::move(set)), pipeline(vkPipeline), pipelineLayout(vkLayout)
    {
    }

    VulkanComputePipeline::~VulkanComputePipeline() noexcept
    {
        if (pipeline != VK_NULL_HANDLE) vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
        if (pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(getDevice().get(), pipelineLayout, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanComputePipelinePtr VulkanComputePipeline::create(const Settings& settings)
    {
        auto [pipeline, layout] = createImpl(settings);
        return std::make_unique<VulkanComputePipeline>(settings, pipeline, layout);
    }

    VulkanComputePipelineSharedPtr VulkanComputePipeline::createShared(const Settings& settings)
    {
        auto [pipeline, layout] = createImpl(settings);
        return std::make_shared<VulkanComputePipeline>(settings, pipeline, layout);
    }

    std::pair<VkPipeline, VkPipelineLayout> VulkanComputePipeline::createImpl(const Settings& settings)
    {
        const auto& device     = settings.computeShader().getDevice();
        const auto  setLayouts = settings.descriptorSetLayouts.get();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts            = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(settings.pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges    = settings.pushConstants.data();

        // Create layout.
        VkPipelineLayout layout;
        handleVulkanError(vkCreatePipelineLayout(device.get(), &pipelineLayoutInfo, nullptr, &layout));

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext              = nullptr;
        pipelineInfo.flags              = 0;
        pipelineInfo.stage.sType        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineInfo.stage.stage        = VK_SHADER_STAGE_COMPUTE_BIT;
        pipelineInfo.stage.module       = settings.computeShader().get();
        pipelineInfo.stage.pName        = "CSMain";
        pipelineInfo.layout             = layout;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex  = 0;

        // Create pipeline.
        VkPipeline pipeline;
        handleVulkanError(vkCreateComputePipelines(device.get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

        return {pipeline, layout};
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanComputePipeline::Settings& VulkanComputePipeline::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanComputePipeline::getDevice() noexcept { return settings.computeShader().getDevice(); }

    const VulkanDevice& VulkanComputePipeline::getDevice() const noexcept
    {
        return settings.computeShader().getDevice();
    }

    const VkPipeline& VulkanComputePipeline::getPipeline() const noexcept { return pipeline; }

    const VkPipelineLayout& VulkanComputePipeline::getPipelineLayout() const noexcept { return pipelineLayout; }
}  // namespace sol