#include "sol-core/vulkan_ray_tracing_pipeline.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
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
                                                       const VkPipelineLayout vkLayout) :
        settings(std::move(set)), pipeline(vkPipeline), pipelineLayout(vkLayout)
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
        auto [pipeline, layout] = createImpl(settings);
        return std::make_unique<VulkanRayTracingPipeline>(settings, pipeline, layout);
    }

    VulkanRayTracingPipelineSharedPtr VulkanRayTracingPipeline::createShared(const Settings& settings)
    {
        auto [pipeline, layout] = createImpl(settings);
        return std::make_shared<VulkanRayTracingPipeline>(settings, pipeline, layout);
    }

    std::tuple<VkPipeline, VkPipelineLayout> VulkanRayTracingPipeline::createImpl(const Settings& settings)
    {
        auto& device     = settings.raygenShader().getDevice();
        auto  setLayouts = settings.descriptorSetLayouts.get();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts            = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(settings.pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges    = settings.pushConstants.data();

        // Create layout.
        VkPipelineLayout layout;
        handleVulkanError(vkCreatePipelineLayout(device.get(), &pipelineLayoutInfo, nullptr, &layout));

        /* Shader stages are laid out like this:
           [
            raygen,
            miss,
            closesthit_0, ..., closesthit_A-1,
            anyhit_0, ..., anyhit_B-1,
            intersection_0, ..., intersection_C-1,
            callable_0, ..., callable_D-1
           ]
           where
           A = size of closestHitShaders
           B = size of anyHitShaders
           C = size of intersectionShaders
           D = size of callableShaders
        */
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        /* Shader groups are laid out like this:
           [
            raygen
            miss
            hit_0, ..., hit_A-1
            callable_0, ..., callable_B
           ]
           where
           A = size of hitGroups
           B = size of callableShaders
         */
        std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups;

        // Create raygen stage.
        {
            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
            stage.module = settings.raygenShader;
            stage.pName  = settings.raygenEntryPoint.empty() ? "main" : settings.raygenEntryPoint.c_str();
            shaderStages.emplace_back(stage);
        }

        // Create miss stages.
        const auto missShaders = settings.missShaders.get();
        for (size_t i = 0; i < missShaders.size(); i++)
        {
            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_MISS_BIT_KHR;
            stage.module = missShaders[i];
            stage.pName  = i < settings.missEntryPoints.size() && !settings.missEntryPoints[i].empty() ?
                             settings.missEntryPoints[i].c_str() :
                             "main";
            shaderStages.emplace_back(stage);
        }

        // Create closest hit stages.
        const auto closestHitShaders = settings.closestHitShaders.get();
        for (size_t i = 0; i < closestHitShaders.size(); i++)
        {
            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
            stage.module = closestHitShaders[i];
            stage.pName  = i < settings.closestHitEntryPoints.size() && !settings.closestHitEntryPoints[i].empty() ?
                             settings.closestHitEntryPoints[i].c_str() :
                             "main";
            shaderStages.emplace_back(stage);
        }

        // Create any hit stages.
        const auto anyHitShaders = settings.anyHitShaders.get();
        for (size_t i = 0; i < anyHitShaders.size(); i++)
        {
            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
            stage.module = anyHitShaders[i];
            stage.pName  = i < settings.anyHitEntryPoints.size() && !settings.anyHitEntryPoints[i].empty() ?
                             settings.anyHitEntryPoints[i].c_str() :
                             "main";
            shaderStages.emplace_back(stage);
        }

        // Create intersection stages.
        const auto intersectionShaders     = settings.intersectionShaders.get();
        const auto intersectionEntryPoints = stringVectorToConstCharVector(settings.intersectionEntryPoints);
        for (size_t i = 0; i < intersectionShaders.size(); i++)
        {
            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
            stage.module = intersectionShaders[i];
            stage.pName  = i < intersectionEntryPoints.size() && !settings.intersectionEntryPoints[i].empty() ?
                             intersectionEntryPoints[i] :
                             "main";
            shaderStages.emplace_back(stage);
        }

        // Create callable stages.
        const auto callableShaders     = settings.callableShaders.get();
        const auto callableEntryPoints = stringVectorToConstCharVector(settings.callableEntryPoints);
        for (size_t i = 0; i < callableShaders.size(); i++)
        {
            VkPipelineShaderStageCreateInfo stage{};
            stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.stage  = VK_SHADER_STAGE_CALLABLE_BIT_KHR;
            stage.module = callableShaders[i];
            stage.pName  = i < callableEntryPoints.size() && !settings.callableEntryPoints[i].empty() ?
                             callableEntryPoints[i] :
                             "main";
            shaderStages.emplace_back(stage);
        }

        constexpr uint32_t raygenOffset       = 0;
        constexpr uint32_t missOffset         = raygenOffset + 1;
        const uint32_t     closestHitOffset   = missOffset + static_cast<uint32_t>(missShaders.size());
        const uint32_t     anyHitOffset       = closestHitOffset + static_cast<uint32_t>(closestHitShaders.size());
        const uint32_t     intersectionOffset = anyHitOffset + static_cast<uint32_t>(anyHitShaders.size());
        const uint32_t     callableOffset     = intersectionOffset + static_cast<uint32_t>(intersectionShaders.size());

        // Create raygen group.
        {
            VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
            shaderGroup.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            shaderGroup.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            shaderGroup.generalShader      = raygenOffset;
            shaderGroup.closestHitShader   = VK_SHADER_UNUSED_KHR;
            shaderGroup.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
            shaderGroups.emplace_back(shaderGroup);
        }

        // Create miss groups.
        for (auto shader : std::views::iota(missOffset, missOffset + static_cast<uint32_t>(missShaders.size())))
        {
            VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
            shaderGroup.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            shaderGroup.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            shaderGroup.generalShader      = shader;
            shaderGroup.closestHitShader   = VK_SHADER_UNUSED_KHR;
            shaderGroup.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
            shaderGroups.emplace_back(shaderGroup);
        }

        // Create hit groups.
        for (const auto& [closest, any, intersection] : settings.hitGroups)
        {
            VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
            shaderGroup.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            shaderGroup.type               = intersection ? VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR :
                                                            VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
            shaderGroup.generalShader      = VK_SHADER_UNUSED_KHR;
            shaderGroup.closestHitShader   = closest ? (closestHitOffset + *closest) : VK_SHADER_UNUSED_KHR;
            shaderGroup.anyHitShader       = any ? (anyHitOffset + *any) : VK_SHADER_UNUSED_KHR;
            shaderGroup.intersectionShader = intersection ? (intersectionOffset + *intersection) : VK_SHADER_UNUSED_KHR;
            shaderGroups.emplace_back(shaderGroup);
        }

        // Create callable groups.
        for (auto shader :
             std::views::iota(callableOffset, callableOffset + static_cast<uint32_t>(callableShaders.size())))
        {
            VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
            shaderGroup.sType              = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            shaderGroup.type               = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            shaderGroup.generalShader      = shader;
            shaderGroup.closestHitShader   = VK_SHADER_UNUSED_KHR;
            shaderGroup.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
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

        return {pipeline, layout};
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

    const VkPipeline& VulkanRayTracingPipeline::get() const noexcept { return pipeline; }

    const VkPipeline& VulkanRayTracingPipeline::getPipeline() const noexcept { return pipeline; }

    const VkPipelineLayout& VulkanRayTracingPipeline::getPipelineLayout() const noexcept { return pipelineLayout; }

    uint32_t VulkanRayTracingPipeline::getRaygenGroupCount() const noexcept { return 1; }

    uint32_t VulkanRayTracingPipeline::getMissGroupCount() const noexcept
    {
        return static_cast<uint32_t>(settings.missShaders.size());
    }

    uint32_t VulkanRayTracingPipeline::getHitGroupCount() const noexcept
    {
        return static_cast<uint32_t>(settings.hitGroups.size());
    }

    uint32_t VulkanRayTracingPipeline::getCallableGroupCount() const noexcept
    {
        return static_cast<uint32_t>(settings.callableShaders.size());
    }
}  // namespace sol
