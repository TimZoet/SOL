#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/fwd.h"
#include "sol-material/fwd.h"
#include "sol-mesh/fwd.h"

namespace sol
{
    class GraphicsRenderData
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Drawable
        {
            /**
             * \brief Mesh instance.
             */
            const Mesh* mesh = nullptr;

            /**
             * \brief Active material.
             */
            const GraphicsMaterial2* material = nullptr;

            /**
             * \brief Offset into the descriptor list.
             */
            size_t descriptorOffset = 0;

            /**
             * \brief Offset into the push constant ranges list.
             */
            size_t pushConstantOffset = 0;

            /**
             * \brief Offset into the dynamic state reference list.
             */
            size_t dynamicStateOffset = 0;

            auto operator<=>(const Drawable&) const noexcept = default;
        };

        struct PushConstantRange
        {
            /**
             * \brief Offset into the push constant data array.
             */
            size_t offset = ~0ULL;

            /**
             * \brief Shader stages that will be updated.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;

            auto operator<=>(const PushConstantRange&) const noexcept = default;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsRenderData();

        GraphicsRenderData(const GraphicsRenderData&) = delete;

        GraphicsRenderData(GraphicsRenderData&&) = delete;

        ~GraphicsRenderData() noexcept;

        GraphicsRenderData& operator=(const GraphicsRenderData&) = delete;

        GraphicsRenderData& operator=(GraphicsRenderData&&) = delete;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void clear();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<Drawable> drawables;

        std::vector<const Descriptor*> descriptors;

        std::vector<PushConstantRange> pushConstantRanges;

        std::vector<std::byte> pushConstantData;

        std::vector<GraphicsDynamicStatePtr> dynamicStates;

        std::vector<const GraphicsDynamicState*> dynamicStateReferences;
    };
}  // namespace sol