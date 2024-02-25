#include "sol-core/vulkan_graphics_pipeline2.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <set>
#include <ranges>

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
#include "sol-core/vulkan_pipeline_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipeline2::VulkanGraphicsPipeline2(Settings set, const VkPipeline vkPipeline) :
        settings(set), pipeline(vkPipeline)
    {
        std::set<VkDynamicState> states;
        if (set.vertexInputPipeline) states.insert_range(settings->vertexInputPipeline().getDynamicStates());
        if (set.preRasterizationPipeline) states.insert_range(settings->preRasterizationPipeline().getDynamicStates());
        if (set.fragmentPipeline) states.insert_range(settings->fragmentPipeline().getDynamicStates());
        if (set.fragmentOutputPipeline) states.insert_range(settings->fragmentOutputPipeline().getDynamicStates());
        dynamicStates      = std::ranges::to<std::vector>(states);
        pushConstantRanges = settings->fragmentPipeline().getSettings().layout().getSettings().pushConstants;
    }

    VulkanGraphicsPipeline2::VulkanGraphicsPipeline2(Settings2 set, const VkPipeline vkPipeline) :
        settings2(set), pipeline(vkPipeline)
    {
        std::set<VkDynamicState> states;
        states.insert_range(settings2->vertexInput.enabledDynamicStates);
        states.insert_range(settings2->preRasterization.enabledDynamicStates);
        states.insert_range(settings2->fragment.enabledDynamicStates);
        states.insert_range(settings2->fragmentOutput.enabledDynamicStates);
        dynamicStates      = std::ranges::to<std::vector>(states);
        pushConstantRanges = settings2->fragment.layout().getSettings().pushConstants;
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

    VulkanGraphicsPipeline2Ptr VulkanGraphicsPipeline2::create2(const Settings2& settings)
    {
        auto pipeline = createImpl2(settings);
        return std::make_unique<VulkanGraphicsPipeline2>(settings, pipeline);
    }

    VulkanGraphicsPipeline2SharedPtr VulkanGraphicsPipeline2::createShared(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipeline2>(settings, pipeline);
    }

    VulkanGraphicsPipeline2SharedPtr VulkanGraphicsPipeline2::createShared2(const Settings2& settings)
    {
        auto pipeline = createImpl2(settings);
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

    VkPipeline VulkanGraphicsPipeline2::createImpl2(const Settings2& settings)
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount =
          static_cast<uint32_t>(settings.vertexInput.vertexAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions = settings.vertexInput.vertexAttributes.data();
        vertexInputInfo.vertexBindingDescriptionCount =
          static_cast<uint32_t>(settings.vertexInput.vertexBindings.size());
        vertexInputInfo.pVertexBindingDescriptions = settings.vertexInput.vertexBindings.data();

        VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
        assemblyInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyInfo.topology               = settings.vertexInput.topology;
        assemblyInfo.primitiveRestartEnable = settings.vertexInput.primitiveRestartEnable;

        std::vector<VkShaderModuleCreateInfo>        shaderModules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        // Reserving for stable pointers.
        shaderModules.reserve(10);

        const auto addStage = [&shaderModules,
                               &shaderStages](const VulkanGraphicsPipelinePreRasterization::Settings::Shader& shader,
                                              const VkShaderStageFlagBits                                     flag) {
            auto& module    = shaderModules.emplace_back();
            module.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            module.flags    = shader.moduleFlags;
            module.codeSize = shader.code.size();
            module.pCode    = reinterpret_cast<const uint32_t*>(shader.code.data());

            auto& stage = shaderStages.emplace_back();
            stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.pNext = &module;
            stage.flags = shader.stageFlags;
            stage.stage = flag;
            stage.pName = shader.entrypoint.c_str();
        };

        if (!settings.preRasterization.vertexShader.code.empty())
            addStage(settings.preRasterization.vertexShader, VK_SHADER_STAGE_VERTEX_BIT);
        if (!settings.preRasterization.tessellationControlShader.code.empty())
            addStage(settings.preRasterization.tessellationControlShader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
        if (!settings.preRasterization.tessellationEvaluationShader.code.empty())
            addStage(settings.preRasterization.tessellationEvaluationShader,
                     VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
        if (!settings.preRasterization.geometryShader.code.empty())
            addStage(settings.preRasterization.geometryShader, VK_SHADER_STAGE_GEOMETRY_BIT);

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.pNext         = nullptr;
        viewportInfo.flags         = settings.preRasterization.viewport.flags;
        viewportInfo.viewportCount = static_cast<uint32_t>(settings.preRasterization.viewport.viewports.size());
        viewportInfo.pViewports    = settings.preRasterization.viewport.viewports.data();
        viewportInfo.scissorCount  = static_cast<uint32_t>(settings.preRasterization.viewport.scissors.size());
        viewportInfo.pScissors     = settings.preRasterization.viewport.scissors.data();

        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        rasterizationInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.pNext                   = nullptr;
        rasterizationInfo.depthClampEnable        = settings.preRasterization.rasterization.depthClampEnable;
        rasterizationInfo.rasterizerDiscardEnable = settings.preRasterization.rasterization.rasterizerDiscardEnable;
        rasterizationInfo.polygonMode             = settings.preRasterization.rasterization.polygonMode;
        rasterizationInfo.cullMode                = settings.preRasterization.rasterization.cullMode;
        rasterizationInfo.frontFace               = settings.preRasterization.rasterization.frontFace;
        rasterizationInfo.depthBiasEnable         = settings.preRasterization.rasterization.depthBiasEnable;
        rasterizationInfo.depthBiasConstantFactor = settings.preRasterization.rasterization.depthBiasConstantFactor;
        rasterizationInfo.depthBiasClamp          = settings.preRasterization.rasterization.depthBiasClamp;
        rasterizationInfo.depthBiasSlopeFactor    = settings.preRasterization.rasterization.depthBiasSlopeFactor;
        rasterizationInfo.lineWidth               = settings.preRasterization.rasterization.lineWidth;

        VkPipelineTessellationStateCreateInfo tessellationInfo{};
        tessellationInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationInfo.patchControlPoints = settings.preRasterization.tessellation.patchControlPoints;

        if (!settings.fragment.fragmentShader.code.empty())
            addStage(settings.fragment.fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT);

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        depthStencilInfo.pNext                 = nullptr;
        depthStencilInfo.flags                 = settings.fragment.depthStencil.flags;
        depthStencilInfo.depthTestEnable       = settings.fragment.depthStencil.depthTestEnable;
        depthStencilInfo.depthWriteEnable      = settings.fragment.depthStencil.depthWriteEnable;
        depthStencilInfo.depthCompareOp        = settings.fragment.depthStencil.depthCompareOp;
        depthStencilInfo.depthBoundsTestEnable = settings.fragment.depthStencil.depthBoundsTestEnable;
        depthStencilInfo.stencilTestEnable     = settings.fragment.depthStencil.stencilTestEnable;
        depthStencilInfo.front                 = settings.fragment.depthStencil.front;
        depthStencilInfo.back                  = settings.fragment.depthStencil.back;
        depthStencilInfo.minDepthBounds        = settings.fragment.depthStencil.minDepthBounds;
        depthStencilInfo.maxDepthBounds        = settings.fragment.depthStencil.maxDepthBounds;

        VkPipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingInfo.pNext = nullptr;
        renderingInfo.colorAttachmentCount =
          static_cast<uint32_t>(settings.fragmentOutput.colorAttachmentFormats.size());
        renderingInfo.pColorAttachmentFormats = settings.fragmentOutput.colorAttachmentFormats.data();
        renderingInfo.depthAttachmentFormat   = settings.fragmentOutput.depthAttachmentFormat;
        renderingInfo.stencilAttachmentFormat = settings.fragmentOutput.stencilAttachmentFormat;

        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        multisampleInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.flags                 = settings.fragmentOutput.multisample.flags;
        multisampleInfo.rasterizationSamples  = settings.fragmentOutput.multisample.rasterizationSamples;
        multisampleInfo.sampleShadingEnable   = settings.fragmentOutput.multisample.sampleShadingEnable;
        multisampleInfo.minSampleShading      = settings.fragmentOutput.multisample.minSampleShading;
        multisampleInfo.pSampleMask           = settings.fragmentOutput.multisample.sampleMasks.data();
        multisampleInfo.alphaToCoverageEnable = settings.fragmentOutput.multisample.alphaToCoverageEnable;
        multisampleInfo.alphaToOneEnable      = settings.fragmentOutput.multisample.alphaToOneEnable;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.flags           = settings.fragmentOutput.colorBlend.flags;
        colorBlendInfo.logicOpEnable   = settings.fragmentOutput.colorBlend.logicOpEnable;
        colorBlendInfo.logicOp         = settings.fragmentOutput.colorBlend.logicOp;
        colorBlendInfo.attachmentCount = static_cast<uint32_t>(settings.fragmentOutput.colorBlend.attachments.size());
        colorBlendInfo.pAttachments    = settings.fragmentOutput.colorBlend.attachments.data();

        std::vector<VkDynamicState> dynamicStates;
        dynamicStates.append_range(settings.vertexInput.enabledDynamicStates);
        dynamicStates.append_range(settings.preRasterization.enabledDynamicStates);
        dynamicStates.append_range(settings.fragment.enabledDynamicStates);
        dynamicStates.append_range(settings.fragmentOutput.enabledDynamicStates);

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext             = VK_NULL_HANDLE;
        dynamicStateInfo.flags             = 0;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates    = dynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext               = &renderingInfo;
        pipelineInfo.flags               = 0;
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &assemblyInfo;
        pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages             = shaderStages.data();
        pipelineInfo.pViewportState      = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pDepthStencilState  = &depthStencilInfo;
        pipelineInfo.pMultisampleState   = &multisampleInfo;
        pipelineInfo.pColorBlendState    = &colorBlendInfo;
        pipelineInfo.pDynamicState       = &dynamicStateInfo;
        pipelineInfo.layout              = settings.fragment.layout;

        // Create pipeline.
        VkPipeline pipeline;
        handleVulkanError(
          vkCreateGraphicsPipelines(settings.vertexInput.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

        return pipeline;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanGraphicsPipeline2::Settings& VulkanGraphicsPipeline2::getSettings() const noexcept { return *settings; }

    const VulkanGraphicsPipeline2::Settings2& VulkanGraphicsPipeline2::getSettings2() const noexcept
    {
        return *settings2;
    }
#endif

    VulkanDevice& VulkanGraphicsPipeline2::getDevice() noexcept
    {
        return settings ? settings->device() : settings2->vertexInput.device();
    }

    const VulkanDevice& VulkanGraphicsPipeline2::getDevice() const noexcept
    {
        return settings ? settings->device() : settings2->vertexInput.device();
    }

    const VkPipeline& VulkanGraphicsPipeline2::get() const noexcept { return pipeline; }

    const VulkanPipelineLayout& VulkanGraphicsPipeline2::getPipelineLayout() const noexcept
    {
        if (settings) return settings->preRasterizationPipeline().getSettings().layout();
        return settings2->preRasterization.layout();
    }

    const std::vector<VkDynamicState>& VulkanGraphicsPipeline2::getDynamicStates() const noexcept
    {
        return dynamicStates;
    }

    const std::vector<VkPushConstantRange>& VulkanGraphicsPipeline2::getPushConstantRanges() const noexcept
    {
        return pushConstantRanges;
    }
}  // namespace sol
