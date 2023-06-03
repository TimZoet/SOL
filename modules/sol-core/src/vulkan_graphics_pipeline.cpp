#include "sol-core/vulkan_graphics_pipeline.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_render_pass.h"
#include "sol-core/vulkan_shader_module.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(Settings               set,
                                                   const VkPipeline       vkPipeline,
                                                   const VkPipelineLayout vkLayout) :
        settings(std::move(set)), pipeline(vkPipeline), pipelineLayout(vkLayout)
    {
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline() noexcept { destroy(); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelinePtr VulkanGraphicsPipeline::create(const Settings& settings)
    {
        auto [pipeline, layout] = createImpl(settings);
        return std::make_unique<VulkanGraphicsPipeline>(settings, pipeline, layout);
    }

    VulkanGraphicsPipelineSharedPtr VulkanGraphicsPipeline::createShared(const Settings& settings)
    {
        auto [pipeline, layout] = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipeline>(settings, pipeline, layout);
    }

    std::pair<VkPipeline, VkPipelineLayout> VulkanGraphicsPipeline::createImpl(const Settings& settings)
    {
        auto& device = settings.renderPass().getDevice();

        // Create layout.
        VkPipelineLayout layout;
        {
            const auto setLayouts = settings.descriptorSetLayouts.get();

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(setLayouts.size());
            pipelineLayoutInfo.pSetLayouts            = setLayouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(settings.pushConstants.size());
            pipelineLayoutInfo.pPushConstantRanges    = settings.pushConstants.data();

            handleVulkanError(vkCreatePipelineLayout(device.get(), &pipelineLayoutInfo, nullptr, &layout));
        }

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        // TODO: Entry point (same for fragment, compute, RT) should be configurable. Store in VulkanShaderModule? Or does a SPIR-V module allow multiple entry points?
        // TODO: Specialization constants.
        // TODO: More stages.

        if (settings.vertexShader)
        {

            shaderStages.emplace_back(VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                      nullptr,
                                      0,
                                      VK_SHADER_STAGE_VERTEX_BIT,
                                      settings.vertexShader,
                                      settings.vertexEntryPoint.c_str(),
                                      nullptr);
        }

        if (settings.fragmentShader)
        {
            shaderStages.emplace_back(VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                      nullptr,
                                      0,
                                      VK_SHADER_STAGE_FRAGMENT_BIT,
                                      settings.fragmentShader,
                                      settings.fragmentEntryPoint.c_str(),
                                      nullptr);
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(settings.vertexAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions    = settings.vertexAttributes.data();
        vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(settings.vertexBindings.size());
        vertexInputInfo.pVertexBindingDescriptions      = settings.vertexBindings.data();

        // Override viewports and scissors.
        VkPipelineViewportStateCreateInfo viewport = settings.viewport;
        if (!settings.viewports.empty())
        {
            viewport.viewportCount = static_cast<uint32_t>(settings.viewports.size());
            viewport.pViewports    = settings.viewports.data();
        }
        if (!settings.scissors.empty())
        {
            viewport.scissorCount = static_cast<uint32_t>(settings.scissors.size());
            viewport.pScissors    = settings.scissors.data();
        }

        // Override attachments.
        VkPipelineColorBlendStateCreateInfo colorBlend = settings.colorBlend;
        if (!settings.colorBlendAttachments.empty())
        {
            colorBlend.attachmentCount = static_cast<uint32_t>(settings.colorBlendAttachments.size());
            colorBlend.pAttachments    = settings.colorBlendAttachments.data();
        }

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pNext             = VK_NULL_HANDLE;
        dynamicState.flags             = 0;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(settings.enabledDynamicStates.size());
        dynamicState.pDynamicStates    = settings.enabledDynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext               = nullptr;
        pipelineInfo.flags               = 0;
        pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages             = shaderStages.data();
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &settings.inputAssembly;
        pipelineInfo.pViewportState      = &viewport;
        pipelineInfo.pRasterizationState = &settings.rasterization;
        pipelineInfo.pMultisampleState   = &settings.multisample;
        pipelineInfo.pDepthStencilState  = &settings.depthStencil;
        pipelineInfo.pColorBlendState    = &colorBlend;
        pipelineInfo.pDynamicState       = &dynamicState;
        pipelineInfo.layout              = layout;
        pipelineInfo.renderPass          = settings.renderPass;
        pipelineInfo.subpass             = 0;
        pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex   = 0;

        // Create pipeline.
        VkPipeline pipeline;
        handleVulkanError(
          vkCreateGraphicsPipelines(device.get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

        return {pipeline, layout};
    }

    void VulkanGraphicsPipeline::destroy()
    {
        // Destroy pipeline and layout.
        if (pipeline != VK_NULL_HANDLE) vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
        if (pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(getDevice().get(), pipelineLayout, nullptr);

        // Clear handles.
        pipeline       = VK_NULL_HANDLE;
        pipelineLayout = VK_NULL_HANDLE;
    }

    void VulkanGraphicsPipeline::recreate()
    {
        if (pipeline != VK_NULL_HANDLE || pipelineLayout != VK_NULL_HANDLE)
            throw SolError("Cannot recreate VulkanGraphicsPipeline before explicitly destroying it.");

        auto [p, l]    = createImpl(settings);
        pipeline       = p;
        pipelineLayout = l;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanGraphicsPipeline::Settings& VulkanGraphicsPipeline::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanGraphicsPipeline::getDevice() noexcept { return settings.renderPass().getDevice(); }

    const VulkanDevice& VulkanGraphicsPipeline::getDevice() const noexcept { return settings.renderPass().getDevice(); }

    const VkPipeline& VulkanGraphicsPipeline::getPipeline() const noexcept { return pipeline; }

    const VkPipelineLayout& VulkanGraphicsPipeline::getPipelineLayout() const noexcept { return pipelineLayout; }
}  // namespace sol