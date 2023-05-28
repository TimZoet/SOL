#include "sol-render/ray_tracing/ray_tracing_renderer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_ray_tracing_pipeline.h"
#include "sol-material/ray_tracing/ray_tracing_material.h"
#include "sol-material/ray_tracing/ray_tracing_material_instance.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/ray_tracing/ray_tracing_material_manager.h"
#include "sol-render/ray_tracing/ray_tracing_render_data.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingRenderer::RayTracingRenderer() = default;

    RayTracingRenderer::~RayTracingRenderer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////
    // Render.
    ////////////////////////////////////////////////////////////////

    void RayTracingRenderer::createPipelines(const Parameters& params) const
    {
        // TODO: RenderData should have a list of all unique materials, instead of looping over the dispatches here.
        // Ensure all pipelines have been created.
        for (const auto& [_a, _b, _c, _d, _e, material, _g] : params.renderData.traces)
        {
            const auto& manager = dynamic_cast<const RayTracingMaterialManager&>(material->getMaterialManager());
            manager.createPipeline(*material);
        }
    }

    void RayTracingRenderer::render(const Parameters& params)
    {
        for (const auto& [raygen, miss, hit, callable, dimensions, material, materialOffset] : params.renderData.traces)
        {
            const auto& materialLayout = material->getLayout();
            const auto& materialManager =
              dynamic_cast<const RayTracingMaterialManager&>(material->getMaterialManager());
            auto& pipeline = materialManager.getPipeline(*material);
            vkCmdBindPipeline(params.commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline.getPipeline());

            {
                std::vector<VkDescriptorSet> sets;
                for (const auto* mtlInstance : params.renderData.materialInstances |
                                                 std::ranges::views::drop(materialOffset) |
                                                 std::ranges::views::take(materialLayout.getSetCount()))
                {
                    const auto& materialManager2 =
                      dynamic_cast<const RayTracingMaterialManager&>(mtlInstance->getMaterialManager());
                    const auto& instanceData = materialManager2.getInstanceData();
                    sets.emplace_back(instanceData.find(mtlInstance)->second->descriptorSets[params.index]);
                }

                vkCmdBindDescriptorSets(params.commandBuffer,
                                        VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
                                        pipeline.getPipelineLayout(),
                                        0,
                                        static_cast<uint32_t>(sets.size()),
                                        sets.data(),
                                        0,
                                        nullptr);
            }

            // TODO: Reverse this push? Might be that this list contains deeper nodes first,
            // causing them to be overwritten by higher nodes when they have an overlapping range.
            // Although perhaps that should be solved in the ForwardRenderData class? Perhaps add a
            // flag there indicating the order of the pcRanges. Or fix order that. While doing that,
            // redundant ranges could even be removed.
            /*for (size_t i = 0; i < pushConstantCount; i++)
            {
                const auto pcRange = params.renderData.pushConstantRanges[pushConstantOffset + i];

                vkCmdPushConstants(params.commandBuffer,
                                   pipeline.getPipelineLayout(),
                                   pcRange.stages,
                                   pcRange.rangeOffset,
                                   pcRange.rangeSize,
                                   params.renderData.pushConstantData.data() + pcRange.offset);
            }*/

            pipeline.getDevice().vkCmdTraceRaysKHR(
              params.commandBuffer, &raygen, &miss, &hit, &callable, dimensions[0], dimensions[1], dimensions[2]);
        }
    }
}  // namespace sol
