#include "sol-core/vulkan_graphics_pipeline_pre_rasterization.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_pipeline_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelinePreRasterization::VulkanGraphicsPipelinePreRasterization(Settings         set,
                                                                                   const VkPipeline vkPipeline) :
        settings(std::move(set)), pipeline(vkPipeline)
    {
    }

    VulkanGraphicsPipelinePreRasterization::~VulkanGraphicsPipelinePreRasterization() noexcept
    {
        vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelinePreRasterizationPtr VulkanGraphicsPipelinePreRasterization::create(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_unique<VulkanGraphicsPipelinePreRasterization>(settings, pipeline);
    }

    VulkanGraphicsPipelinePreRasterizationSharedPtr
      VulkanGraphicsPipelinePreRasterization::createShared(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipelinePreRasterization>(settings, pipeline);
    }

    VkPipeline VulkanGraphicsPipelinePreRasterization::createImpl(const Settings& settings)
    {
        VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
        libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT;
        constexpr auto flags =
          VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;

        std::vector<VkShaderModuleCreateInfo>        shaderModules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        // Reserving for stable pointers.
        shaderModules.reserve(5);

        const auto addStage = [&shaderModules, &shaderStages](const Settings::Shader&     shader,
                                                              const VkShaderStageFlagBits flag) {
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

        if (!settings.vertexShader.code.empty()) addStage(settings.vertexShader, VK_SHADER_STAGE_VERTEX_BIT);
        if (!settings.tessellationControlShader.code.empty())
            addStage(settings.tessellationControlShader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
        if (!settings.tessellationEvaluationShader.code.empty())
            addStage(settings.tessellationEvaluationShader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
        if (!settings.geometryShader.code.empty()) addStage(settings.geometryShader, VK_SHADER_STAGE_GEOMETRY_BIT);

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.pNext         = nullptr;
        viewportInfo.flags         = settings.viewport.flags;
        viewportInfo.viewportCount = static_cast<uint32_t>(settings.viewport.viewports.size());
        viewportInfo.pViewports    = settings.viewport.viewports.data();
        viewportInfo.scissorCount  = static_cast<uint32_t>(settings.viewport.scissors.size());
        viewportInfo.pScissors     = settings.viewport.scissors.data();

        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        rasterizationInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.pNext                   = nullptr;
        rasterizationInfo.depthClampEnable        = settings.rasterization.depthClampEnable;
        rasterizationInfo.rasterizerDiscardEnable = settings.rasterization.rasterizerDiscardEnable;
        rasterizationInfo.polygonMode             = settings.rasterization.polygonMode;
        rasterizationInfo.cullMode                = settings.rasterization.cullMode;
        rasterizationInfo.frontFace               = settings.rasterization.frontFace;
        rasterizationInfo.depthBiasEnable         = settings.rasterization.depthBiasEnable;
        rasterizationInfo.depthBiasConstantFactor = settings.rasterization.depthBiasConstantFactor;
        rasterizationInfo.depthBiasClamp          = settings.rasterization.depthBiasClamp;
        rasterizationInfo.depthBiasSlopeFactor    = settings.rasterization.depthBiasSlopeFactor;
        rasterizationInfo.lineWidth               = settings.rasterization.lineWidth;

        VkPipelineTessellationStateCreateInfo tessellationInfo{};
        tessellationInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationInfo.patchControlPoints = settings.tessellation.patchControlPoints;

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
        pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages             = shaderStages.data();
        pipelineInfo.pViewportState      = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pDynamicState       = &dynamicStateInfo;
        pipelineInfo.layout              = settings.layout;

        // Create pipeline.
        VkPipeline pipeline;
        handleVulkanError(vkCreateGraphicsPipelines(
          settings.layout().getDevice().get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

        return pipeline;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanGraphicsPipelinePreRasterization::Settings&
      VulkanGraphicsPipelinePreRasterization::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanGraphicsPipelinePreRasterization::getDevice() noexcept { return settings.layout().getDevice(); }

    const VulkanDevice& VulkanGraphicsPipelinePreRasterization::getDevice() const noexcept
    {
        return settings.layout().getDevice();
    }

    const VkPipeline& VulkanGraphicsPipelinePreRasterization::get() const noexcept { return pipeline; }

    const std::vector<VkDynamicState>& VulkanGraphicsPipelinePreRasterization::getDynamicStates() const noexcept
    {
        return settings.enabledDynamicStates;
    }
}  // namespace sol