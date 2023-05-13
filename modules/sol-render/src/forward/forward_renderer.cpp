#include "sol-render/forward/forward_renderer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

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
#include "sol-material/forward/forward_material.h"
#include "sol-memory/memory_manager.h"
#include "sol-mesh/i_mesh.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/render_settings.h"
#include "sol-render/forward/forward_material_manager.h"
#include "sol-render/forward/forward_render_data.h"
#include "sol-render/forward/forward_traverser.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardRenderer::ForwardRenderer() : renderSettings(std::make_shared<RenderSettings>()) {}

    ForwardRenderer::ForwardRenderer(RenderSettingsSharedPtr settings) : renderSettings(std::move(settings)) {}

    ForwardRenderer::~ForwardRenderer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    RenderSettingsSharedPtr& ForwardRenderer::getRenderSettings() noexcept { return renderSettings; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ForwardRenderer::setRenderSettings(RenderSettingsSharedPtr settings)
    {
        if (!settings) throw SolError("Cannot set RenderSettings to nullptr.");
        renderSettings = std::move(settings);
    }

    ////////////////////////////////////////////////////////////////
    // Render.
    ////////////////////////////////////////////////////////////////

    void ForwardRenderer::createPipelines(const Parameters& params) const
    {
        // TODO: RenderData should have a list of all unique materials, instead of looping over the drawables here.
        // Ensure all pipelines have been created.
        for (const auto& [mesh, material, materialOffset, _a, _b] : params.renderData.drawables)
        {
            // TODO: Ugh, const_cast.
            auto& manager = const_cast<ForwardMaterialManager&>(
              dynamic_cast<const ForwardMaterialManager&>(material->getMaterialManager()));
            manager.createPipeline(*material, *renderSettings, params.renderPass);
        }
    }

    void ForwardRenderer::beginRenderPass(const Parameters& params) const
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
        renderPassInfo.clearValueCount   = clearValues.size();
        renderPassInfo.pClearValues      = clearValues.data();
        vkCmdBeginRenderPass(params.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void ForwardRenderer::render(const Parameters& params)
    {
        bool                  setDynamicState = false;
        std::vector<VkBuffer> vertexBuffers;
        std::vector<size_t>   vertexBufferOffsets;

        for (const auto& [mesh, material, materialOffset, pushConstantOffset, pushConstantCount] :
             params.renderData.drawables)
        {
            const auto& materialLayout  = material->getLayout();
            const auto& materialManager = dynamic_cast<const ForwardMaterialManager&>(material->getMaterialManager());
            auto&       pipeline        = materialManager.getPipeline(*material, *renderSettings, params.renderPass);
            vkCmdBindPipeline(params.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());

            // Set the dynamic state after binding the first pipeline.
            if (!setDynamicState)
            {
                VkViewport viewport{};
                viewport.x        = 0.0f;
                viewport.y        = 0.0f;
                viewport.width    = static_cast<float>(params.framebuffer.getExtent().width);
                viewport.height   = static_cast<float>(params.framebuffer.getExtent().height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(params.commandBuffer, 0, 1, &viewport);

                VkRect2D scissor{};
                scissor.offset = {0, 0};
                scissor.extent = params.framebuffer.getExtent();
                vkCmdSetScissor(params.commandBuffer, 0, 1, &scissor);

                setDynamicState = true;
            }

            {
                std::vector<VkDescriptorSet> sets;
                for (const auto* mtlInstance : params.renderData.materialInstances |
                                                 std::ranges::views::drop(materialOffset) |
                                                 std::ranges::views::take(materialLayout.getSetCount()))
                {
                    const auto& materialManager2 =
                      dynamic_cast<const ForwardMaterialManager&>(mtlInstance->getMaterialManager());
                    const auto& instanceData = materialManager2.getInstanceData();
                    sets.emplace_back(instanceData.find(mtlInstance)->second->descriptorSets[params.index]);
                }

                vkCmdBindDescriptorSets(params.commandBuffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
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

    void ForwardRenderer::endRenderPass(const Parameters& params) { vkCmdEndRenderPass(params.commandBuffer); }
}  // namespace sol