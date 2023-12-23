#include "sol-core/vulkan_graphics_pipeline2.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_graphics_pipeline_fragment.h"
#include "sol-core/vulkan_graphics_pipeline_fragment_output.h"
#include "sol-core/vulkan_graphics_pipeline_pre_rasterization.h"
#include "sol-core/vulkan_graphics_pipeline_vertex_input.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipeline2::VulkanGraphicsPipeline2(Settings set, const VkPipeline vkPipeline) :
        settings(set), pipeline(vkPipeline)
    {
    }

    VulkanGraphicsPipeline2::~VulkanGraphicsPipeline2() noexcept
    {
        vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipeline2Ptr VulkanGraphicsPipeline2::create(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_unique<VulkanGraphicsPipeline2>(settings, pipeline);
    }

    VulkanGraphicsPipeline2SharedPtr VulkanGraphicsPipeline2::createShared(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipeline2>(settings, pipeline);
    }

    VkPipeline VulkanGraphicsPipeline2::createImpl(const Settings& settings)
    {
        std::vector<VkPipeline> pipelines;
        if (settings.vertexInputPipeline) pipelines.push_back(settings.vertexInputPipeline().get());
        if (settings.preRasterizationPipeline) pipelines.push_back(settings.preRasterizationPipeline().get());
        if (settings.fragmentPipeline) pipelines.push_back(settings.fragmentPipeline().get());
        if (settings.fragmentOutputPipeline) pipelines.push_back(settings.fragmentOutputPipeline().get());

        VkPipelineLibraryCreateInfoKHR linkingInfo{};
        linkingInfo.sType        = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;
        linkingInfo.libraryCount = static_cast<uint32_t>(pipelines.size());
        linkingInfo.pLibraries   = pipelines.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = &linkingInfo;
        pipelineInfo.flags = VK_PIPELINE_CREATE_LINK_TIME_OPTIMIZATION_BIT_EXT;

        // Create pipeline.
        VkPipeline pipeline;
        handleVulkanError(
          vkCreateGraphicsPipelines(settings.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

        return pipeline;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanGraphicsPipeline2::Settings& VulkanGraphicsPipeline2::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanGraphicsPipeline2::getDevice() noexcept { return settings.device(); }

    const VulkanDevice& VulkanGraphicsPipeline2::getDevice() const noexcept { return settings.device(); }

    const VkPipeline& VulkanGraphicsPipeline2::get() const noexcept { return pipeline; }
}  // namespace sol