#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-descriptor/fwd.h"
#include "sol-material/fwd.h"
#include "sol-mesh/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_dynamic_state.h"
#include "sol-render/graphics/fwd.h"

namespace sol
{
    class GraphicsRenderer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Parameters
        {
            const VulkanDevice&       device;
            const GraphicsRenderData& renderData;
            VkCommandBuffer           commandBuffer;
            const uint32_t            index;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsRenderer();

        GraphicsRenderer(const GraphicsRenderer&) = delete;

        GraphicsRenderer(GraphicsRenderer&&) = delete;

        ~GraphicsRenderer() noexcept;

        GraphicsRenderer& operator=(const GraphicsRenderer&) = delete;

        GraphicsRenderer& operator=(GraphicsRenderer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Render.
        ////////////////////////////////////////////////////////////////

        void render(const Parameters& params);

    private:
        void bindDescriptorBuffers(const Parameters& params);

        void bindMaterial(VkCommandBuffer cb, const GraphicsMaterial2& material);

        void bindDynamicStates(VkCommandBuffer          cb,
                               const Parameters&        params,
                               const GraphicsMaterial2& material,
                               size_t                   stateOffset);

        void bindPushConstants(VkCommandBuffer          cb,
                               const Parameters&        params,
                               const GraphicsMaterial2& material,
                               size_t                   pushConstantOffset);

        void bindDescriptors(VkCommandBuffer          cb,
                             const Parameters&        params,
                             const GraphicsMaterial2& material,
                             size_t                   descriptorOffset);

        [[nodiscard]] uint32_t bindIndexBuffer(VkCommandBuffer cb, const Mesh& mesh);

        void bindVertexBuffers(VkCommandBuffer cb, const Mesh& mesh);

        std::vector<const DescriptorBuffer*>                                             activeDescriptorBuffers = {};
        const VulkanGraphicsPipeline2*                                                   activePipeline      = nullptr;
        std::unordered_map<GraphicsDynamicState::StateType, const GraphicsDynamicState*> activeDynamicStates = {};
        std::vector<std::pair<uint32_t, VkDeviceSize>>                                   activeDescriptors   = {};
        const VulkanBuffer*                                                              activeIndexBuffer   = nullptr;
        std::vector<std::pair<const VulkanBuffer*, size_t>>                              activeVertexBuffers = {};
    };
}  // namespace sol
