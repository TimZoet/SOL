#include "sol-render/graphics/graphics_render_data.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderData::GraphicsRenderData() = default;

    GraphicsRenderData::~GraphicsRenderData() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderData::clear()
    {
        drawables.clear();
        materialInstances.clear();
        pushConstantRanges.clear();
        pushConstantData.clear();
    }

    ////////////////////////////////////////////////////////////////
    // Sorting.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderData::sortDrawablesByLayer()
    {
        std::ranges::sort(drawables, [](const Drawable& lhs, const Drawable& rhs) {
            return lhs.material->getLayer() < rhs.material->getLayer();
        });
    }

}  // namespace sol