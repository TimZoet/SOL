#include "sol-material/graphics/graphics_material_instance2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_material2.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsMaterialInstance2::GraphicsMaterialInstance2(const uuids::uuid id, GraphicsMaterial2& mtl) :
        MaterialInstance2(id, mtl)
    {
    }

    GraphicsMaterialInstance2::~GraphicsMaterialInstance2() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    GraphicsMaterial2& GraphicsMaterialInstance2::getGraphicsMaterial() noexcept
    {
        return static_cast<GraphicsMaterial2&>(getMaterial());
    }

    const GraphicsMaterial2& GraphicsMaterialInstance2::getGraphicsMaterial() const noexcept
    {
        return static_cast<const GraphicsMaterial2&>(getMaterial());
    }
}  // namespace sol
