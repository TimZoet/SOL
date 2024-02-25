#include "sol-core/vulkan_graphics_pipeline_vertex_input.h"

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

    VulkanGraphicsPipelineVertexInput::VulkanGraphicsPipelineVertexInput(Settings set, const VkPipeline vkPipeline) :
        settings(std::move(set)), pipeline(vkPipeline)
    {
    }

    VulkanGraphicsPipelineVertexInput::~VulkanGraphicsPipelineVertexInput() noexcept
    {
        vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelineVertexInputPtr VulkanGraphicsPipelineVertexInput::create(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_unique<VulkanGraphicsPipelineVertexInput>(settings, pipeline);
    }

    VulkanGraphicsPipelineVertexInputSharedPtr VulkanGraphicsPipelineVertexInput::createShared(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipelineVertexInput>(settings, pipeline);
    }

    VkPipeline VulkanGraphicsPipelineVertexInput::createImpl(const Settings& settings)
    {
        VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
        libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT;
        constexpr auto flags =
          VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(settings.vertexAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions    = settings.vertexAttributes.data();
        vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(settings.vertexBindings.size());
        vertexInputInfo.pVertexBindingDescriptions      = settings.vertexBindings.data();

        VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
        assemblyInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyInfo.topology               = settings.topology;
        assemblyInfo.primitiveRestartEnable = settings.primitiveRestartEnable;

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext             = VK_NULL_HANDLE;
        dynamicStateInfo.flags             = 0;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(settings.enabledDynamicStates.size());
        dynamicStateInfo.pDynamicStates    = settings.enabledDynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext               = &libraryInfo;
        pipelineInfo.flags               = flags;
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &assemblyInfo;
        pipelineInfo.pDynamicState       = &dynamicStateInfo;

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
    const VulkanGraphicsPipelineVertexInput::Settings& VulkanGraphicsPipelineVertexInput::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanGraphicsPipelineVertexInput::getDevice() noexcept { return settings.device(); }

    const VulkanDevice& VulkanGraphicsPipelineVertexInput::getDevice() const noexcept { return settings.device(); }

    const VkPipeline& VulkanGraphicsPipelineVertexInput::get() const noexcept { return pipeline; }

    const std::vector<VkDynamicState>& VulkanGraphicsPipelineVertexInput::getDynamicStates() const noexcept
    {
        return settings.enabledDynamicStates;
    }
}  // namespace sol