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

    VulkanGraphicsPipeline& GraphicsPipelineCache::getPipeline(const GraphicsMaterial& material) const
    {
        const auto it = pipelines.find(&material);
        if (it == pipelines.end())
            throw SolError("Cannot get pipeline for GraphicsMaterial: no pipelines created yet.");

        return *it->second;
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    bool GraphicsPipelineCache::createPipeline(const GraphicsMaterial& material)
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

    bool GraphicsPipelineCache::destroyPipeline(const GraphicsMaterial& material) { return pipelines.erase(&material); }

    VulkanGraphicsPipelinePtr GraphicsPipelineCache::createPipelineImpl(const GraphicsMaterial& material)
    {
        const auto& layout = material.getGraphicsLayout();

        VulkanGraphicsPipeline::Settings settings;
        settings.vertexShader          = material.getVertexShader();
        settings.fragmentShader        = material.getFragmentShader();
        settings.vertexAttributes      = layout.getMeshLayout()->getAttributeDescriptions();
        settings.vertexBindings        = layout.getMeshLayout()->getBindingDescriptions();
        settings.descriptorSetLayouts  = layout.getDescriptorSetLayouts();
        settings.pushConstants         = layout.getPushConstants();
        settings.colorBlendAttachments = layout.getColorBlendAttachments();

        // Either enable various dynamic states, or initialize pipeline with values from layout.

        if (layout.isDynamicStateEnabled<VK_DYNAMIC_STATE_VIEWPORT>())
        {
            settings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            settings.viewport.viewportCount = layout.getDynamicViewportCount();
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
            settings.viewport.scissorCount = layout.getDynamicScissorCount();
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
            case ECullMode::None: settings.rasterization.cullMode = VK_CULL_MODE_NONE; break;
            case ECullMode::Front: settings.rasterization.cullMode = VK_CULL_MODE_FRONT_BIT; break;
            case ECullMode::Back: settings.rasterization.cullMode = VK_CULL_MODE_BACK_BIT; break;
            case ECullMode::Both: settings.rasterization.cullMode = VK_CULL_MODE_FRONT_AND_BACK; break;
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
            case EFrontFace::Clockwise: settings.rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE; break;
            case EFrontFace::CounterClockwise: settings.rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; break;
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
            case EPolygonMode::Fill: settings.rasterization.polygonMode = VK_POLYGON_MODE_FILL; break;
            case EPolygonMode::Line: settings.rasterization.polygonMode = VK_POLYGON_MODE_LINE; break;
            case EPolygonMode::Point: settings.rasterization.polygonMode = VK_POLYGON_MODE_POINT; break;
            }
        }

        return VulkanGraphicsPipeline::create(settings);
    }
}  // namespace sol
