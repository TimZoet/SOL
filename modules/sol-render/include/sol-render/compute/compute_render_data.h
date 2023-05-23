#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

namespace sol
{
    class ComputeRenderData
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Dispatch
        {
            /**
             * \brief Group count.
             */
            std::array<uint32_t, 3> groupCount{0, 0, 0};

            /**
             * \brief Active material.
             */
            const ComputeMaterial* material = nullptr;

            /**
             * \brief Offset into the material instance list.
             */
            size_t materialOffset = 0;

            /**
             * \brief Offset into the push constant ranges list.
             */
            //size_t pushConstantOffset = 0;

            /**
             * \brief Number of push constants.
             */
            //size_t pushConstantCount = 0;
        };

        //struct PushConstantRange
        //{
        //    /**
        //     * \brief Start offset of the push constant range to update in bytes.
        //     */
        //    size_t rangeOffset = 0;

        //    /**
        //     * \brief Size of the push constant range to update in bytes.
        //     */
        //    size_t rangeSize = 0;

        //    /**
        //     * \brief Offset into the push constant data array.
        //     */
        //    size_t offset = 0;

        //    /**
        //     * \brief Shader stages that will be updated.
        //     */
        //    VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;
        //};

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeRenderData();

        ComputeRenderData(const ComputeRenderData&) = delete;

        ComputeRenderData(ComputeRenderData&&) = delete;

        ~ComputeRenderData() noexcept;

        ComputeRenderData& operator=(const ComputeRenderData&) = delete;

        ComputeRenderData& operator=(ComputeRenderData&&) = delete;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void clear();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<Dispatch> dispatches;

        std::vector<const ComputeMaterialInstance*> materialInstances;

        // TODO: Push constants.
        /**
         * \brief Range into the push constant data array.
         */
        //std::vector<PushConstantRange> pushConstantRanges;

        //std::vector<std::byte> pushConstantData;
    };
}  // namespace sol