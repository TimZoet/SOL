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

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(SettingsPtr            settingsPtr,
                                                   const VkPipeline       vkPipeline,
                                                   const VkPipelineLayout vkLayout) :
        settings(std::move(settingsPtr)), pipeline(vkPipeline), pipelineLayout(vkLayout)
    {
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline() noexcept { destroy(); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelinePtr VulkanGraphicsPipeline::create(Settings settings)
    {
        auto [pipeline, layout] = createImpl(settings);
        return std::make_unique<VulkanGraphicsPipeline>(
          std::make_unique<Settings>(std::move(settings)), pipeline, layout);
    }

    VulkanGraphicsPipelineSharedPtr VulkanGraphicsPipeline::createShared(Settings settings)
    {
        auto [pipeline, layout] = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipeline>(
          std::make_unique<Settings>(std::move(settings)), pipeline, layout);
    }

    std::pair<VkPipeline, VkPipelineLayout> VulkanGraphicsPipeline::createImpl(const Settings& settings)
    {
        auto& device = settings.renderPass().getDevice();

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = settings.vertexShader().get();
        vertShaderStageInfo.pName  = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = settings.fragmentShader;
        fragShaderStageInfo.pName  = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(settings.vertexAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions    = settings.vertexAttributes.data();
        vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(settings.vertexBindings.size());
        vertexInputInfo.pVertexBindingDescriptions      = settings.vertexBindings.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports    = VK_NULL_HANDLE;
        viewportState.scissorCount  = 1;
        viewportState.pScissors     = VK_NULL_HANDLE;

        // Use rasterization info from settings or default.
        VkPipelineRasterizationStateCreateInfo rasterization{};
        if (settings.rasterization)
            rasterization = *settings.rasterization;
        else
        {
            rasterization.sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterization.polygonMode = VK_POLYGON_MODE_FILL;
            rasterization.cullMode    = VK_CULL_MODE_BACK_BIT;
            rasterization.frontFace   = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterization.lineWidth   = 1.0f;
        }

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable  = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Use depthStencil info from settings or default.
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        if (settings.depthStencil)
            depthStencil = *settings.depthStencil;
        else
        {
            depthStencil.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable  = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp   = VK_COMPARE_OP_LESS;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable     = VK_FALSE;
        colorBlending.logicOp           = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount   = static_cast<uint32_t>(settings.colorBlending.size());
        colorBlending.pAttachments      = settings.colorBlending.data();
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        const std::vector                pDynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pNext             = VK_NULL_HANDLE;
        dynamicState.flags             = 0;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
        dynamicState.pDynamicStates    = pDynamicStates.data();

        auto setLayouts = settings.descriptorSetLayouts.get();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts            = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(settings.pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges    = settings.pushConstants.data();

        // Create layout.
        VkPipelineLayout layout;
        handleVulkanError(vkCreatePipelineLayout(device.get(), &pipelineLayoutInfo, nullptr, &layout));

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext               = nullptr;
        pipelineInfo.flags               = 0;
        pipelineInfo.stageCount          = 2;
        pipelineInfo.pStages             = shaderStages;
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pRasterizationState = &rasterization;
        pipelineInfo.pMultisampleState   = &multisampling;
        pipelineInfo.pDepthStencilState  = &depthStencil;
        pipelineInfo.pColorBlendState    = &colorBlending;
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

        auto [p, l]    = createImpl(*settings);
        pipeline       = p;
        pipelineLayout = l;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanGraphicsPipeline::Settings& VulkanGraphicsPipeline::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanGraphicsPipeline::getDevice() noexcept { return settings->renderPass().getDevice(); }

    const VulkanDevice& VulkanGraphicsPipeline::getDevice() const noexcept
    {
        return settings->renderPass().getDevice();
    }

    const VkPipeline& VulkanGraphicsPipeline::getPipeline() const noexcept { return pipeline; }

    const VkPipelineLayout& VulkanGraphicsPipeline::getPipelineLayout() const noexcept { return pipelineLayout; }
}  // namespace sol