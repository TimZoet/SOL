#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
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

        void createPipelines(const Parameters& params) const;

        void render(const Parameters& params);
    };
}  // namespace sol
