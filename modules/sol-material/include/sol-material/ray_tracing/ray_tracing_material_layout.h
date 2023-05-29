#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/material_layout.h"

namespace sol
{
    class RayTracingMaterialLayout final : public MaterialLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingMaterialLayout() = delete;

        explicit RayTracingMaterialLayout(VulkanDevice& vkDevice);

        RayTracingMaterialLayout(const RayTracingMaterialLayout&) = delete;

        RayTracingMaterialLayout(RayTracingMaterialLayout&&) = delete;

        ~RayTracingMaterialLayout() noexcept override;

        RayTracingMaterialLayout& operator=(const RayTracingMaterialLayout&) = delete;

        RayTracingMaterialLayout& operator=(RayTracingMaterialLayout&&) = delete;
    };
}  // namespace sol
