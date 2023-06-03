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
            const IMesh* mesh = nullptr;

            /**
             * \brief Active material.
             */
            const GraphicsMaterial* material = nullptr;

            /**
             * \brief Offset into the material instance list.
             */
            size_t materialOffset = 0;

            /**
             * \brief Offset into the push constant ranges list.
             */
            size_t pushConstantOffset = 0;

            /**
             * \brief Number of push constants.
             */
            size_t pushConstantCount = 0;
        };

        struct PushConstantRange
        {
            /**
             * \brief Start offset of the push constant range to update in bytes.
             */
            uint32_t rangeOffset = 0;

            /**
             * \brief Size of the push constant range to update in bytes.
             */
            uint32_t rangeSize = 0;

            /**
             * \brief Offset into the push constant data array.
             */
            size_t offset = 0;

            /**
             * \brief Shader stages that will be updated.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;
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
        // Sorting.
        ////////////////////////////////////////////////////////////////

        void sortDrawablesByLayer();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<Drawable> drawables;

        std::vector<const GraphicsMaterialInstance*> materialInstances;

        /**
         * \brief Range into the push constant data array.
         */
        std::vector<PushConstantRange> pushConstantRanges;

        std::vector<std::byte> pushConstantData;
    };
}  // namespace sol