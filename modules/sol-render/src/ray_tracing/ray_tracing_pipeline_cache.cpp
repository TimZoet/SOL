#include "sol-render/ray_tracing/ray_tracing_pipeline_cache.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_ray_tracing_pipeline.h"
#include "sol-error/sol_error.h"
#include "sol-material/ray_tracing/ray_tracing_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingPipelineCache::RayTracingPipelineCache() = default;

    RayTracingPipelineCache::~RayTracingPipelineCache() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanRayTracingPipeline& RayTracingPipelineCache::getPipeline(const RayTracingMaterial& material) const
    {
        const auto it = pipelines.find(&material);
        if (it == pipelines.end())
            throw SolError("Cannot get pipeline for RayTracingMaterial: no pipeline created yet.");
        return *it->second.pipeline;
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    bool RayTracingPipelineCache::createPipeline(const RayTracingMaterial& material)
    {
        // Look for existing pipeline list for this material.
        const auto it = pipelines.find(&material);

        // None found, create new pipeline.
        if (it == pipelines.end())
        {
            pipelines.try_emplace(&material, createPipelineImpl(material));
            return true;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////
    // Destroy.
    ////////////////////////////////////////////////////////////////

    bool RayTracingPipelineCache::destroyPipeline(const RayTracingMaterial& material)
    {
        return pipelines.erase(&material);
    }

    VulkanRayTracingPipelinePtr RayTracingPipelineCache::createPipelineImpl(const RayTracingMaterial& material)
    {
        VulkanRayTracingPipeline::Settings settings;
        settings.raygenShader = material.getRaygenShader();
        settings.missShaders += material.getMissShaders();
        settings.closestHitShaders += material.getClosestHitShaders();
        settings.anyHitShaders += material.getAnyHitShaders();
        settings.intersectionShaders += material.getIntersectionShaders();
        settings.callableShaders += material.getCallableShaders();
        settings.hitGroups = material.getHitGroups();
        // TODO: Entry points.
        settings.descriptorSetLayouts = material.getLayout().getDescriptorSetLayouts();
        // TODO: Push constants.

        return VulkanRayTracingPipeline::create(settings);
    }
}  // namespace sol
