#include "sol-render/graphics/graphics_render_data.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_dynamic_state.h"

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
        descriptors.clear();
        pushConstantRanges.clear();
        pushConstantData.clear();
        dynamicStates.clear();
        dynamicStateReferences.clear();
    }

}  // namespace sol
