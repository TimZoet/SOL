#include "sol-render/compute/compute_pipeline_cache.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_compute_pipeline.h"
#include "sol-error/sol_error.h"
#include "sol-material/compute/compute_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputePipelineCache::ComputePipelineCache() = default;

    ComputePipelineCache::~ComputePipelineCache() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanComputePipeline& ComputePipelineCache::getPipeline(const ComputeMaterial& material) const
    {
        const auto it = pipelines.find(&material);
        if (it == pipelines.end()) throw SolError("Cannot get pipeline for ComputeMaterial: no pipeline created yet.");
        return *it->second.pipeline;
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    bool ComputePipelineCache::createPipeline(const ComputeMaterial& material)
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

    bool ComputePipelineCache::destroyPipeline(const ComputeMaterial& material) { return pipelines.erase(&material); }

    VulkanComputePipelinePtr ComputePipelineCache::createPipelineImpl(const ComputeMaterial& material)
    {
        VulkanComputePipeline::Settings settings;
        settings.computeShader        = const_cast<VulkanShaderModule&>(material.getComputeShader());//TODO: const_cast
        settings.descriptorSetLayouts = material.getLayout().getFinalizedDescriptorSetLayouts();
        // TODO: Push constants.

        return VulkanComputePipeline::create(settings);
    }
}  // namespace sol
