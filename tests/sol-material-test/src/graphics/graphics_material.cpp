#include "sol-material-test/graphics/graphics_material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/utils/projections.h"
#include "sol-material/graphics/graphics_material2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "testutils/materials.h"

void GraphicsMaterial::operator()()
{
    const auto [descriptorLayouts, material] = Materials::load(Materials::Graphics::Name::Simple, getDevice());

    compareEQ(descriptorLayouts, material->getDescriptorLayouts(), bt::proj::unique_ptr_to_raw_ptr<sol::DescriptorLayout>);

    // Test dynamic states.
    compareTrue(material->isDynamicStateEnabled(sol::GraphicsDynamicState::StateType::Scissor));
    compareTrue(material->isDynamicStateEnabled(sol::GraphicsDynamicState::StateType::Viewport));
    compareEQ(2, material->getDynamicStates().size());
    expectThrow([&] { static_cast<void>(material->createDynamicState<sol::CullMode>()); });
    expectNoThrow([&] { static_cast<void>(material->createDynamicState<sol::Viewport>()); });
}
