#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

namespace sol
{
    class RayTracingRenderData
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Trace
        {
            VkStridedDeviceAddressRegionKHR raygenSBT{};

            VkStridedDeviceAddressRegionKHR missSBT{};

            VkStridedDeviceAddressRegionKHR hitSBT{};

            VkStridedDeviceAddressRegionKHR callableSBT{};

            /**
             * \brief Dimensions.
             */
            std::array<uint32_t, 3> dimensions{0, 0, 0};

            /**
             * \brief Active material.
             */
            const RayTracingMaterial* material = nullptr;

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

        RayTracingRenderData();

        RayTracingRenderData(const RayTracingRenderData&) = delete;

        RayTracingRenderData(RayTracingRenderData&&) = delete;

        ~RayTracingRenderData() noexcept;

        RayTracingRenderData& operator=(const RayTracingRenderData&) = delete;

        RayTracingRenderData& operator=(RayTracingRenderData&&) = delete;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void clear();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<Trace> traces;

        std::vector<const RayTracingMaterialInstance*> materialInstances;

        // TODO: Push constants.
        /**
         * \brief Range into the push constant data array.
         */
        //std::vector<PushConstantRange> pushConstantRanges;

        //std::vector<std::byte> pushConstantData;
    };
}  // namespace sol