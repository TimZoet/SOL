#include "sol-render/graphics/graphics_renderer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>
#include <set>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_pipeline_layout.h"
#include "sol-descriptor/descriptor.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-mesh/index_buffer.h"
#include "sol-mesh/mesh.h"
#include "sol-mesh/vertex_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/graphics/graphics_render_data.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderer::GraphicsRenderer() = default;

    GraphicsRenderer::~GraphicsRenderer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Render.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderer::render(const Parameters& params)
    {
        // Reset active state.
        activeDescriptorBuffers.clear();
        activePipeline = nullptr;
        activeDynamicStates.clear();
        activeDescriptors.clear();
        activeIndexBuffer = nullptr;
        activeVertexBuffers.clear();

        bindDescriptorBuffers(params);

        for (const auto& [mesh, material, descriptorOffset, pushConstantOffset, dynamicStateOffset] :
             params.renderData.drawables)
        {
            bindMaterial(params.commandBuffer, *material);
            bindDynamicStates(params.commandBuffer, params, *material, dynamicStateOffset);
            bindPushConstants(params.commandBuffer, params, *material, pushConstantOffset);
            bindDescriptors(params.commandBuffer, params, *material, descriptorOffset);
            const auto firstIndex = bindIndexBuffer(params.commandBuffer, *mesh);
            bindVertexBuffers(params.commandBuffer, *mesh);
            if (mesh->hasIndexBuffer())
                vkCmdDrawIndexed(params.commandBuffer, 0, 1, firstIndex, 0, 0);
            else
                vkCmdDraw(params.commandBuffer, 0, 1, 0, 0);
        }
    }

    void GraphicsRenderer::bindDescriptorBuffers(const Parameters& params)
    {
        std::set<const DescriptorBuffer*> uniqueBuffers;

        for (const auto& [mesh, material, descriptorOffset, pushConstantOffset, dynamicStateOffset] :
             params.renderData.drawables)
        {
            for (size_t i = 0; i < material->getDescriptorLayouts().size(); i++)
            {
                const auto& buffer = params.renderData.descriptors[descriptorOffset + i]->getBuffer();
                uniqueBuffers.insert(&buffer);
            }
        }

        activeDescriptorBuffers = uniqueBuffers | std::ranges::to<std::vector>();
        const auto infos =
          activeDescriptorBuffers | std::views::transform([](const DescriptorBuffer* b) {
              return VkDescriptorBufferBindingInfoEXT{.sType   = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT,
                                                      .pNext   = nullptr,
                                                      .address = b->getBuffer().getDeviceAddress(),
                                                      .usage   = b->getBuffer().getSettings().bufferUsage};
          }) |
          std::ranges::to<std::vector>();

        // TODO: Verify that activeDescriptorBuffers.size() <= VK_EXT_descriptor_buffer.maxDescriptorBufferBindings.

        params.device.vkCmdBindDescriptorBuffersEXT(
          params.commandBuffer, static_cast<uint32_t>(activeDescriptorBuffers.size()), infos.data());
    }

    void GraphicsRenderer::bindMaterial(const VkCommandBuffer cb, const GraphicsMaterial2& material)
    {
        // Rebind the pipeline if it is different or not yet set.
        if (&material.getPipeline() != activePipeline)
        {
            vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, material.getPipeline().get());
            activePipeline = &material.getPipeline();

            // Clear active descriptors.
            // TODO: Is this necessary when using descriptor buffers? With descriptor pools you had the fun pipeline layout compatibility stuff.
            // Does that apply here as well?
            activeDescriptors.clear();
        }
    }

    void GraphicsRenderer::bindDynamicStates(const VkCommandBuffer    cb,
                                             const Parameters&        params,
                                             const GraphicsMaterial2& material,
                                             const size_t             stateOffset)
    {
        for (size_t i = 0; i < material.getDynamicStates().size(); i++)
        {

            const auto& state = *params.renderData.dynamicStateReferences[stateOffset + i];
            const auto  it    = activeDynamicStates.find(state.getType());

            if (it == activeDynamicStates.end() || it->second != &state)
            {
                activeDynamicStates.insert_or_assign(it, state.getType(), &state);

                switch (state.getType())
                {
                case GraphicsDynamicState::StateType::CullMode:
                    vkCmdSetCullMode(cb, toVulkanEnum(static_cast<const CullMode&>(state).value));
                    break;
                case GraphicsDynamicState::StateType::FrontFace:
                    vkCmdSetFrontFace(cb, toVulkanEnum(static_cast<const FrontFace&>(state).value));
                    break;
                case GraphicsDynamicState::StateType::PolygonMode:
                    params.device.vkCmdSetPolygonModeEXT(cb,
                                                         toVulkanEnum(static_cast<const PolygonMode&>(state).value));
                    break;
                case GraphicsDynamicState::StateType::Scissor: {
                    const auto& scissor = static_cast<const Scissor&>(state);
                    vkCmdSetScissorWithCount(cb,
                                             static_cast<uint32_t>(scissor.values.size()),
                                             reinterpret_cast<const VkRect2D*>(scissor.values.data()));
                    break;
                }
                case GraphicsDynamicState::StateType::Viewport: {
                    const auto& viewport = static_cast<const Viewport&>(state);
                    vkCmdSetViewportWithCount(cb,
                                              static_cast<uint32_t>(viewport.values.size()),
                                              reinterpret_cast<const VkViewport*>(viewport.values.data()));
                    break;
                }
                default: break;
                }
            }
        }
    }

    void GraphicsRenderer::bindPushConstants(const VkCommandBuffer    cb,
                                             const Parameters&        params,
                                             const GraphicsMaterial2& material,
                                             const size_t             pushConstantOffset)
    {
        // TODO: This is inefficient because we currently re-push all constants without checking what is already bound.
        // This could be optimized with memcmp / hashing, though that might be difficult with nodes having potentially differing stage flags.
        // TODO: Additionally, it may be wrong, because the stage flags are potentially not taken into account properly.
        // Maybe get rid of nodes being able to provide only a subset of stageflags? Cool feature, but how often do you really want to have different push constants between stages in the same draw call?
        for (size_t i = 0; i < material.getPushConstantRanges().size(); i++)
        {
            const auto& data  = params.renderData.pushConstantRanges[pushConstantOffset + i];
            const auto& range = material.getPushConstantRanges()[i];

            vkCmdPushConstants(cb,
                               material.getPipeline().getPipelineLayout().get(),
                               range.stageFlags,
                               range.offset,
                               range.size,
                               params.renderData.pushConstantData.data() + data.offset);
        }
    }

    void GraphicsRenderer::bindDescriptors(const VkCommandBuffer    cb,
                                           const Parameters&        params,
                                           const GraphicsMaterial2& material,
                                           const size_t             descriptorOffset)
    {
        if (activeDescriptors.size() < material.getDescriptorLayouts().size())
            activeDescriptors.resize(material.getDescriptorLayouts().size());

        std::vector<uint32_t>     bufferIndices(material.getDescriptorLayouts().size());
        std::vector<VkDeviceSize> offsets(material.getDescriptorLayouts().size());
        size_t                    firstRebindIndex = ~0ULL;

        for (size_t i = 0; i < material.getDescriptorLayouts().size(); i++)
        {
            const auto& desc   = *params.renderData.descriptors[descriptorOffset + i];
            const auto  index  = static_cast<uint32_t>(std::distance(
              activeDescriptorBuffers.begin(), std::ranges::find(activeDescriptorBuffers, &desc.getBuffer())));
            const auto  offset = desc.getOffset();

            if (activeDescriptors[i].first != index || activeDescriptors[i].second != offset)
            {
                activeDescriptors[i] = {index, offset};
                if (firstRebindIndex == ~0ULL) firstRebindIndex = i;
            }

            bufferIndices[i] = index;
            offsets[i]       = offset;
        }

        if (firstRebindIndex != ~0ULL)
        {
            params.device.vkCmdSetDescriptorBufferOffsetsEXT(
              cb,
              VK_PIPELINE_BIND_POINT_GRAPHICS,
              material.getPipeline().getPipelineLayout().get(),
              static_cast<uint32_t>(firstRebindIndex),
              static_cast<uint32_t>(material.getDescriptorLayouts().size() - firstRebindIndex),
              bufferIndices.data() + firstRebindIndex,
              offsets.data() + firstRebindIndex);
        }
    }

    uint32_t GraphicsRenderer::bindIndexBuffer(const VkCommandBuffer cb, const Mesh& mesh)
    {
        if (mesh.hasIndexBuffer())
        {
            if (activeIndexBuffer != &mesh.getIndexBuffer()->getBuffer())
            {
                // We bind the index buffer with an offset of 0, instead relying on the getIndexOffset() value during the draw call.
                vkCmdBindIndexBuffer(
                  cb, mesh.getIndexBuffer()->getBuffer().get(), 0, mesh.getIndexBuffer()->getIndexType());
                activeIndexBuffer = &mesh.getIndexBuffer()->getBuffer();
            }

            return static_cast<uint32_t>(mesh.getIndexBuffer()->getIndexOffset());
        }

        return 0;
    }

    void GraphicsRenderer::bindVertexBuffers(const VkCommandBuffer cb, const Mesh& mesh)
    {
        if (activeVertexBuffers.size() < mesh.getVertexBufferCount())
            activeVertexBuffers.resize(mesh.getVertexBufferCount());

        for (size_t i = 0; i < mesh.getVertexBufferCount(); i++)
        {
            const std::pair<const VulkanBuffer*, size_t> vertexBuffer = {&mesh.getVertexBuffers()[i]->getBuffer(),
                                                                         mesh.getVertexBuffers()[i]->getBufferOffset()};

            if (activeVertexBuffers[i] != vertexBuffer)
            {
                const auto buffer = vertexBuffer.first->get();
                const auto offset = vertexBuffer.second;
                vkCmdBindVertexBuffers(cb, static_cast<uint32_t>(i), 1, &buffer, &offset);

                activeVertexBuffers[i] = vertexBuffer;
            }
        }
    }
}  // namespace sol
