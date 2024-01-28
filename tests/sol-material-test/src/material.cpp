#include "sol-material-test/material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/utils/projections.h"
#include "sol-material/graphics/graphics_material2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "testutils/materials.h"

void Material::operator()()
{
    const auto [descriptorLayouts, material] = Materials::load(Materials::Graphics::Name::Simple, getDevice());

    compareEQ(
      descriptorLayouts, material->getDescriptorLayouts(), bt::proj::unique_ptr_to_raw_ptr<sol::DescriptorLayout>);

    compareEQ(&getDevice(), &material->getDevice());
    compareNE(uuids::uuid{}, material->getUuid());
}
