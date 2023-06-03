#include "sol-render/graphics/graphics_pipeline_cache.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_graphics_pipeline.h"
#include "sol-error/sol_error.h"
#include "sol-material/graphics/graphics_material.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_render_pass.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsPipelineCache::GraphicsPipelineCache() = default;

    GraphicsPipelineCache::~GraphicsPipelineCache() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipeline& GraphicsPipelineCache::getPipeline(const GraphicsMaterial& material,
                                                               const VulkanRenderPass& renderPass) const
    {
        const auto it = pipelines.find(&material);
        if (it == pipelines.end())
            throw SolError("Cannot get pipeline for GraphicsMaterial: no pipelines created yet.");

        for (const auto& obj : it->second)
        {
            if (obj.renderPass == &renderPass) return *obj.pipeline;
        }

        throw SolError(
          "Cannot get pipeline for GraphicsMaterial: no pipeline with compatible settings and renderpass found.");
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    bool GraphicsPipelineCache::createPipeline(const GraphicsMaterial& material, VulkanRenderPass& renderPass)
    {
        // Look for existing pipeline list for this material.
        const auto it = pipelines.find(&material);

        // None found, create whole new list with new pipeline.
        if (it == pipelines.end())
        {
            std::vector<Pipeline> p;
            p.emplace_back(Pipeline{.pipeline = createPipelineImpl(material, renderPass), .renderPass = &renderPass});
            pipelines.try_emplace(&material, std::move(p));
            return true;
        }

        // Look for pipeline with same settings.
        for (const auto& obj : it->second)
        {
            if (obj.renderPass == &renderPass) return false;
        }

        // Create new pipeline and add to list.
        it->second.emplace_back(
          Pipeline{.pipeline = createPipelineImpl(material, renderPass), .renderPass = &renderPass});

        return true;
    }

    ////////////////////////////////////////////////////////////////
    // Destroy.
    ////////////////////////////////////////////////////////////////

    bool GraphicsPipelineCache::destroyPipeline(const GraphicsMaterial& material)
    {
        // TODO: This destroys all pipelines for this material, regardless of rendersettings and renderpass.
        return pipelines.erase(&material);
    }

    VulkanGraphicsPipelinePtr GraphicsPipelineCache::createPipelineImpl(const GraphicsMaterial& material,
                                                                        VulkanRenderPass&       renderPass)
    {
        const auto* meshLayout = material.getMeshLayout();
        if (!meshLayout) throw SolError("Cannot create pipeline: material has no mesh layout.");

        const auto& layout = material.getGraphicsLayout();

        VulkanGraphicsPipeline::Settings settings;
        settings.renderPass            = renderPass;
        settings.vertexShader          = material.getVertexShader();
        settings.fragmentShader        = material.getFragmentShader();
        settings.vertexAttributes      = meshLayout->getAttributeDescriptions();
        settings.vertexBindings        = meshLayout->getBindingDescriptions();
        settings.descriptorSetLayouts  = layout.getDescriptorSetLayouts();
        settings.pushConstants         = layout.getPushConstants();
        settings.colorBlendAttachments = layout.getColorBlendAttachments();

        // Either enable various dynamic states, or initialize pipeline with values from layout.

        if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_VIEWPORT>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        }
        else if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);
        }
        else
        {
            throw SolError("");
            // TODO:
            //pipelineSettings.viewports.emplace_back(...);
        }

        if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_SCISSOR>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
        }
        else if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);
        }
        else
        {
            throw SolError("");
            // TODO:
            //pipelineSettings.scissors.emplace_back(...);
        }

        if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_CULL_MODE>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_CULL_MODE);
        }
        else
        {
            switch (layout.getCullMode())
            {
            case CullMode::None: settings.rasterization.cullMode = VK_CULL_MODE_NONE; break;
            case CullMode::Front: settings.rasterization.cullMode = VK_CULL_MODE_FRONT_BIT; break;
            case CullMode::Back: settings.rasterization.cullMode = VK_CULL_MODE_BACK_BIT; break;
            case CullMode::Both: settings.rasterization.cullMode = VK_CULL_MODE_FRONT_AND_BACK; break;
            }
        }

        if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_FRONT_FACE>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_FRONT_FACE);
        }
        else
        {
            switch (layout.getFrontFace())
            {
            case FrontFace::Clockwise: settings.rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE; break;
            case FrontFace::CounterClockwise: settings.rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; break;
            }
        }

        if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_POLYGON_MODE_EXT>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_POLYGON_MODE_EXT);
        }
        else
        {
            switch (layout.getPolygonMode())
            {
            case PolygonMode::Fill: settings.rasterization.polygonMode = VK_POLYGON_MODE_FILL; break;
            case PolygonMode::Line: settings.rasterization.polygonMode = VK_POLYGON_MODE_LINE; break;
            case PolygonMode::Point: settings.rasterization.polygonMode = VK_POLYGON_MODE_POINT; break;
            }
        }

        return VulkanGraphicsPipeline::create(settings);
    }
}  // namespace sol
