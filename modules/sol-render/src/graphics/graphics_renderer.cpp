#include "sol-render/graphics/graphics_renderer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_attachment.h"
#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_frame_buffer.h"
#include "sol-core/vulkan_graphics_pipeline.h"
#include "sol-core/vulkan_render_pass.h"
#include "sol-error/sol_error.h"
#include "sol-material/graphics/graphics_material.h"
#include "sol-memory/memory_manager.h"
#include "sol-mesh/i_mesh.h"
////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/render_settings.h"
#include "sol-render/graphics/graphics_material_manager.h"
#include "sol-render/graphics/graphics_render_data.h"
#include "sol-render/graphics/graphics_traverser.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderer::GraphicsRenderer() : renderSettings(std::make_shared<RenderSettings>()) {}

    GraphicsRenderer::GraphicsRenderer(RenderSettingsSharedPtr settings) : renderSettings(std::move(settings)) {}

    GraphicsRenderer::~GraphicsRenderer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    RenderSettingsSharedPtr& GraphicsRenderer::getRenderSettings() noexcept { return renderSettings; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderer::setRenderSettings(RenderSettingsSharedPtr settings)
    {
        if (!settings) throw SolError("Cannot set RenderSettings to nullptr.");
        renderSettings = std::move(settings);
    }

    ////////////////////////////////////////////////////////////////
    // Render.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderer::createPipelines(const Parameters& params) const
    {
        // TODO: RenderData should have a list of all unique materials, instead of looping over the drawables here.
        // Ensure all pipelines have been created.
        for (const auto& [mesh, material, materialOffset, _a, _b] : params.renderData.drawables)
        {
            material->getMaterialManager().createPipeline(*material, params.renderPass);
        }
    }

    void GraphicsRenderer::beginRenderPass(const Parameters& params) const
    {
        VkRenderPassBeginInfo       renderPassInfo{};
        std::array<VkClearValue, 2> clearValues{};
        switch (renderSettings->getClearColorFormat())
        {
        case RenderSettings::ClearColorFormat::Float:
            const auto ccf = renderSettings->getClearColorFloat();
            std::ranges::copy(ccf.begin(), ccf.end(), clearValues[0].color.float32);
            break;
        case RenderSettings::ClearColorFormat::Int:
            const auto cci = renderSettings->getClearColorInt();
            std::ranges::copy(cci.begin(), cci.end(), clearValues[0].color.int32);
            break;
        case RenderSettings::ClearColorFormat::Uint:
            const auto ccu = renderSettings->getClearColorUint();
            std::ranges::copy(ccu.begin(), ccu.end(), clearValues[0].color.uint32);
            break;
        }
        clearValues[1].depthStencil      = {renderSettings->getClearDepth(), renderSettings->getClearStencil()};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = params.renderPass.get();
        renderPassInfo.framebuffer       = params.framebuffer.get();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = params.framebuffer.getExtent();
        renderPassInfo.clearValueCount   = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues      = clearValues.data();
        vkCmdBeginRenderPass(params.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void GraphicsRenderer::render(const Parameters& params)
    {
        const auto& device = params.framebuffer.getDevice();

        std::vector<VkBuffer> vertexBuffers;
        std::vector<size_t>   vertexBufferOffsets;

        for (const auto& [mesh, material, materialOffset, pushConstantOffset, pushConstantCount] :
             params.renderData.drawables)
        {
            const auto& materialLayout  = material->getGraphicsLayout();
            const auto& materialManager = material->getMaterialManager();
            auto&       pipeline        = materialManager.getPipeline(*material, params.renderPass);
            vkCmdBindPipeline(params.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());

            // Just bind all dynamic states for now.
            for (size_t i = 0; i < materialLayout.getSetCount(); i++)
            {
                const auto& inst = *params.renderData.materialInstances[materialOffset + i];
                if (materialLayout.isDynamicStateEnabled<VK_DYNAMIC_STATE_VIEWPORT>())
                {
                    if (const auto& viewports = inst.getViewports(); viewports)
                    {
                        vkCmdSetViewport(
                          params.commandBuffer, 0, static_cast<uint32_t>(viewports->size()), viewports->data());
                    }
                }

                if (materialLayout.isDynamicStateEnabled<VK_DYNAMIC_STATE_SCISSOR>())
                {
                    if (const auto& scissors = inst.getScissors(); scissors)
                    {
                        vkCmdSetScissor(
                          params.commandBuffer, 0, static_cast<uint32_t>(scissors->size()), scissors->data());
                    }
                }

                if (materialLayout.isDynamicStateEnabled<VK_DYNAMIC_STATE_CULL_MODE>())
                {
                    if (const auto cull = inst.getCullMode(); cull)
                    {
                        vkCmdSetCullMode(params.commandBuffer, toVulkanEnum(*cull));
                    }
                }

                if (materialLayout.isDynamicStateEnabled<VK_DYNAMIC_STATE_FRONT_FACE>())
                {
                    if (const auto face = inst.getFrontFace(); face)
                    {
                        vkCmdSetFrontFace(params.commandBuffer, toVulkanEnum(*face));
                    }
                }

                if (materialLayout.isDynamicStateEnabled<VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT>())
                {
                    if (const auto& viewports = inst.getViewports(); viewports)
                    {
                        vkCmdSetViewportWithCount(params.commandBuffer, 0, viewports->data());
                    }
                }

                if (materialLayout.isDynamicStateEnabled<VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT>())
                {
                    if (const auto& scissors = inst.getScissors(); scissors)
                    {
                        vkCmdSetScissorWithCount(params.commandBuffer, 0, scissors->data());
                    }
                }

                if (materialLayout.isDynamicStateEnabled<VK_DYNAMIC_STATE_POLYGON_MODE_EXT>())
                {
                    if (const auto polygon = inst.getPolygonMode(); polygon)
                    {
                        device.vkCmdSetPolygonModeEXT(params.commandBuffer, toVulkanEnum(*polygon));
                    }
                }
            }

            materialManager.bindDescriptorSets(
              {params.renderData.materialInstances.data() + materialOffset, materialLayout.getSetCount()},
              params.commandBuffer,
              pipeline,
              params.index);

            // TODO: Reverse this push? Might be that this list contains deeper nodes first,
            // causing them to be overwritten by higher nodes when they have an overlapping range.
            // Although perhaps that should be solved in the GraphicsRenderData class? Perhaps add a
            // flag there indicating the order of the pcRanges. Or fix order that. While doing that,
            // redundant ranges could even be removed.
            for (size_t i = 0; i < pushConstantCount; i++)
            {
                const auto pcRange = params.renderData.pushConstantRanges[pushConstantOffset + i];

                vkCmdPushConstants(params.commandBuffer,
                                   pipeline.getPipelineLayout(),
                                   pcRange.stages,
                                   pcRange.rangeOffset,
                                   pcRange.rangeSize,
                                   params.renderData.pushConstantData.data() + pcRange.offset);
            }


            const auto& meshInstance = mesh;

            if (!meshInstance->isValid()) continue;

            vertexBuffers.clear();
            vertexBufferOffsets.clear();
            meshInstance->getVertexBufferHandles(vertexBuffers);
            meshInstance->getVertexBufferOffsets(vertexBufferOffsets);
            assert(vertexBuffers.size() == vertexBufferOffsets.size());

            vkCmdBindVertexBuffers(params.commandBuffer,
                                   0,
                                   static_cast<uint32_t>(vertexBuffers.size()),
                                   vertexBuffers.data(),
                                   vertexBufferOffsets.data());
            if (meshInstance->isIndexed())
            {
                vkCmdBindIndexBuffer(params.commandBuffer,
                                     meshInstance->getIndexBufferHandle(),
                                     meshInstance->getIndexBufferOffset(),
                                     meshInstance->getIndexType());
                vkCmdDrawIndexed(params.commandBuffer,
                                 meshInstance->getIndexCount(),
                                 1,
                                 meshInstance->getFirstIndex(),
                                 meshInstance->getVertexOffset(),
                                 0);
            }
            else
            {
                vkCmdDraw(params.commandBuffer, meshInstance->getVertexCount(), 1, meshInstance->getFirstVertex(), 0);
            }
        }
    }

    void GraphicsRenderer::endRenderPass(const Parameters& params) { vkCmdEndRenderPass(params.commandBuffer); }
}  // namespace sol