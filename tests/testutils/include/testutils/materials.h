#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-descriptor/fwd.h"
#include "sol-material/graphics/graphics_material2.h"

class Materials
{
public:
    struct Graphics
    {
        enum class Name
        {
            Simple
        };

        std::vector<sol::DescriptorLayoutPtr> descriptorLayouts;
        sol::GraphicsMaterial2Ptr             material;
    };


    [[nodiscard]] static Graphics load(Graphics::Name name, sol::VulkanDevice& device);
};
