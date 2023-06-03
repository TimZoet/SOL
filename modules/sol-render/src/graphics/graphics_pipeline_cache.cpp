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

#include "sol-render/common/render_settings.h"

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

        // TODO: To what extent are the RenderSettings needed here? Things like culling should perhaps be put in the GraphicsMaterial class instead.
        VulkanGraphicsPipeline::Settings pipelineSettings;
        pipelineSettings.renderPass   = renderPass;
        pipelineSettings.vertexShader = const_cast<VulkanShaderModule&>(material.getVertexShader());  //TODO: const_cast
        pipelineSettings.fragmentShader       = const_cast<VulkanShaderModule&>(material.getFragmentShader());
        pipelineSettings.vertexAttributes     = meshLayout->getAttributeDescriptions();
        pipelineSettings.vertexBindings       = meshLayout->getBindingDescriptions();
        pipelineSettings.descriptorSetLayouts = material.getLayout().getDescriptorSetLayouts();
        pipelineSettings.pushConstants        = material.getLayout().getPushConstants();
        pipelineSettings.colorBlending        = material.getGraphicsLayout().getColorBlending();

        VkPipelineRasterizationStateCreateInfo rasterization{};
        rasterization.sType     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.lineWidth = 1.0f;
        switch (material.getPolyonMode())
        {
        case GraphicsMaterial::PolygonMode::Fill: rasterization.polygonMode = VK_POLYGON_MODE_FILL; break;
        case GraphicsMaterial::PolygonMode::Line: rasterization.polygonMode = VK_POLYGON_MODE_LINE; break;
        case GraphicsMaterial::PolygonMode::Point: rasterization.polygonMode = VK_POLYGON_MODE_POINT; break;
        }
        switch (material.getCullMode())
        {
        case GraphicsMaterial::CullMode::None: rasterization.cullMode = VK_CULL_MODE_NONE; break;
        case GraphicsMaterial::CullMode::Front: rasterization.cullMode = VK_CULL_MODE_FRONT_BIT; break;
        case GraphicsMaterial::CullMode::Back: rasterization.cullMode = VK_CULL_MODE_BACK_BIT; break;
        case GraphicsMaterial::CullMode::Both: rasterization.cullMode = VK_CULL_MODE_FRONT_AND_BACK; break;
        }
        switch (material.getFrontFace())
        {
        case GraphicsMaterial::FrontFace::Clockwise: rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE; break;
        case GraphicsMaterial::FrontFace::CounterClockwise:
            rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
        }

        pipelineSettings.rasterization = rasterization;

        return VulkanGraphicsPipeline::create(pipelineSettings);
    }
}  // namespace sol
