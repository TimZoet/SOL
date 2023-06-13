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
    class ComputeMaterialLayout final : public MaterialLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeMaterialLayout() = delete;

        ComputeMaterialLayout(VulkanDevice& device);

        ComputeMaterialLayout(const ComputeMaterialLayout&) = delete;

        ComputeMaterialLayout(ComputeMaterialLayout&&) = delete;

        ~ComputeMaterialLayout() noexcept override;

        ComputeMaterialLayout& operator=(const ComputeMaterialLayout&) = delete;

        ComputeMaterialLayout& operator=(ComputeMaterialLayout&&) = delete;
    };
}  // namespace sol
