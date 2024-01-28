#include "sol-material-test/graphics/graphics_material_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-material/graphics/graphics_material_instance2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "testutils/materials.h"

void GraphicsMaterialInstance::operator()()
{
    const auto [descriptorLayouts, material] = Materials::load(Materials::Graphics::Name::Simple, getDevice());

    // Create material instance.
    sol::GraphicsMaterialInstance2Ptr instance;
    expectNoThrow([&] { instance = material->createInstance<sol::GraphicsMaterialInstance2>(); });
    compareEQ(material.get(), &instance->getGraphicsMaterial());
}
