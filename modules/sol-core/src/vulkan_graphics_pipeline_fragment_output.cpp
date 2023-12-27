#include "sol-core/vulkan_graphics_pipeline_fragment_output.h"

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

    VulkanGraphicsPipelineFragmentOutput::VulkanGraphicsPipelineFragmentOutput(Settings         set,
                                                                               const VkPipeline vkPipeline) :
        settings(std::move(set)), pipeline(vkPipeline)
    {
    }

    VulkanGraphicsPipelineFragmentOutput::~VulkanGraphicsPipelineFragmentOutput() noexcept
    {
        vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelineFragmentOutputPtr VulkanGraphicsPipelineFragmentOutput::create(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_unique<VulkanGraphicsPipelineFragmentOutput>(settings, pipeline);
    }

    VulkanGraphicsPipelineFragmentOutputSharedPtr
      VulkanGraphicsPipelineFragmentOutput::createShared(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipelineFragmentOutput>(settings, pipeline);
    }

    VkPipeline VulkanGraphicsPipelineFragmentOutput::createImpl(const Settings& settings)
    {
        VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
        libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT;
        constexpr auto flags =
          VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;

        VkPipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingInfo.pNext                   = &libraryInfo;
        renderingInfo.colorAttachmentCount    = static_cast<uint32_t>(settings.colorAttachmentFormats.size());
        renderingInfo.pColorAttachmentFormats = settings.colorAttachmentFormats.data();
        renderingInfo.depthAttachmentFormat   = settings.depthAttachmentFormat;
        renderingInfo.stencilAttachmentFormat = settings.stencilAttachmentFormat;

        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        multisampleInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.flags                 = settings.multisample.flags;
        multisampleInfo.rasterizationSamples  = settings.multisample.rasterizationSamples;
        multisampleInfo.sampleShadingEnable   = settings.multisample.sampleShadingEnable;
        multisampleInfo.minSampleShading      = settings.multisample.minSampleShading;
        multisampleInfo.pSampleMask           = settings.multisample.sampleMasks.data();
        multisampleInfo.alphaToCoverageEnable = settings.multisample.alphaToCoverageEnable;
        multisampleInfo.alphaToOneEnable      = settings.multisample.alphaToOneEnable;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.flags           = settings.colorBlend.flags;
        colorBlendInfo.logicOpEnable   = settings.colorBlend.logicOpEnable;
        colorBlendInfo.logicOp         = settings.colorBlend.logicOp;
        colorBlendInfo.attachmentCount = static_cast<uint32_t>(settings.colorBlend.attachments.size());
        colorBlendInfo.pAttachments    = settings.colorBlend.attachments.data();

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext             = VK_NULL_HANDLE;
        dynamicStateInfo.flags             = 0;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(settings.enabledDynamicStates.size());
        dynamicStateInfo.pDynamicStates    = settings.enabledDynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType             = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext             = &renderingInfo;
        pipelineInfo.flags             = flags;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pColorBlendState  = &colorBlendInfo;
        pipelineInfo.pDynamicState     = &dynamicStateInfo;

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
    const VulkanGraphicsPipelineFragmentOutput::Settings&
      VulkanGraphicsPipelineFragmentOutput::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanGraphicsPipelineFragmentOutput::getDevice() noexcept { return settings.device(); }

    const VulkanDevice& VulkanGraphicsPipelineFragmentOutput::getDevice() const noexcept { return settings.device(); }

    const VkPipeline& VulkanGraphicsPipelineFragmentOutput::get() const noexcept { return pipeline; }

    const std::vector<VkDynamicState>& VulkanGraphicsPipelineFragmentOutput::getDynamicStates() const noexcept
    {
        return settings.enabledDynamicStates;
    }
}  // namespace sol
