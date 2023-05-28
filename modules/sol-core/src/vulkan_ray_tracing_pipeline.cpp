#include "sol-core/vulkan_ray_tracing_pipeline.h"

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

    VulkanRayTracingPipeline::VulkanRayTracingPipeline(Settings               set,
                                                       const VkPipeline       vkPipeline,
                                                       const VkPipelineLayout vkLayout,
                                                       const uint32_t         sGroupCount) :
        settings(std::move(set)), pipeline(vkPipeline), pipelineLayout(vkLayout), shaderGroupCount(sGroupCount)
    {
    }

    VulkanRayTracingPipeline::~VulkanRayTracingPipeline() noexcept
    {
        if (pipeline != VK_NULL_HANDLE) vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
        if (pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(getDevice().get(), pipelineLayout, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanRayTracingPipelinePtr VulkanRayTracingPipeline::create(const Settings& settings)
    {
        auto [pipeline, layout, shaderGroupCount] = createImpl(settings);
        return std::make_unique<VulkanRayTracingPipeline>(settings, pipeline, layout, shaderGroupCount);
    }

    VulkanRayTracingPipelineSharedPtr VulkanRayTracingPipeline::createShared(const Settings& settings)
    {
        auto [pipeline, layout, shaderGroupCount] = createImpl(settings);
        return std::make_shared<VulkanRayTracingPipeline>(settings, pipeline, layout, shaderGroupCount);
    }

    std::tuple<VkPipeline, VkPipelineLayout, uint32_t> VulkanRayTracingPipeline::createImpl(const Settings& settings)
    {
        auto& device = settings.raygenShader().getDevice();


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

        std::vector<VkPipelineShaderStageCreateInfo>      shaderStages;
        std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups;
        {
            VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
            shaderGroup.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            shaderGroup.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            shaderGroup.generalShader      = static_cast<uint32_t>(shaderStages.size());
            shaderGroup.closestHitShader   = VK_SHADER_UNUSED_KHR;
            shaderGroup.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
            shaderGroups.emplace_back(shaderGroup);

            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
            stage.module = settings.raygenShader;
            stage.pName  = "main";
            shaderStages.emplace_back(stage);
        }

        {
            VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
            shaderGroup.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            shaderGroup.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            shaderGroup.generalShader      = static_cast<uint32_t>(shaderStages.size());
            shaderGroup.closestHitShader   = VK_SHADER_UNUSED_KHR;
            shaderGroup.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
            shaderGroups.emplace_back(shaderGroup);

            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_MISS_BIT_KHR;
            stage.module = settings.missShader;
            stage.pName  = "main";
            shaderStages.emplace_back(stage);
        }

        {
            VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
            shaderGroup.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            shaderGroup.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
            shaderGroup.generalShader      = VK_SHADER_UNUSED_KHR;
            shaderGroup.closestHitShader   = static_cast<uint32_t>(shaderStages.size());
            shaderGroup.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
            stage.module = settings.closestHitShader;
            stage.pName  = "main";
            shaderStages.emplace_back(stage);

            if (settings.anyHitShader)
            {
                shaderGroup.anyHitShader = static_cast<uint32_t>(shaderStages.size());

                stage.stage  = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
                stage.module = settings.anyHitShader;
                shaderStages.emplace_back(stage);
            }

            if (settings.intersectionShader)
            {
                shaderGroup.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
                shaderGroup.intersectionShader = static_cast<uint32_t>(shaderStages.size());

                stage.stage  = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
                stage.module = settings.intersectionShader;
                shaderStages.emplace_back(stage);
            }

            shaderGroups.emplace_back(shaderGroup);
        }

        // Create pipeline.
        VkPipeline                        pipeline;
        VkRayTracingPipelineCreateInfoKHR pipelineInfo{};
        pipelineInfo.sType                        = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
        pipelineInfo.stageCount                   = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages                      = shaderStages.data();
        pipelineInfo.groupCount                   = static_cast<uint32_t>(shaderGroups.size());
        pipelineInfo.pGroups                      = shaderGroups.data();
        pipelineInfo.maxPipelineRayRecursionDepth = 1;
        pipelineInfo.layout                       = layout;
        handleVulkanError(device.vkCreateRayTracingPipelinesKHR(
          device.get(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

        return {pipeline, layout, static_cast<uint32_t>(shaderGroups.size())};
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanRayTracingPipeline::Settings& VulkanRayTracingPipeline::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanRayTracingPipeline::getDevice() noexcept { return settings.raygenShader().getDevice(); }

    const VulkanDevice& VulkanRayTracingPipeline::getDevice() const noexcept
    {
        return settings.raygenShader().getDevice();
    }

    const VkPipeline& VulkanRayTracingPipeline::getPipeline() const noexcept { return pipeline; }

    const VkPipelineLayout& VulkanRayTracingPipeline::getPipelineLayout() const noexcept { return pipelineLayout; }

    uint32_t VulkanRayTracingPipeline::getShaderGroupCount() const noexcept { return shaderGroupCount; }
}  // namespace sol
