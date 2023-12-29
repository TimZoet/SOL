#include "sol-material-test/material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_graphics_pipeline2.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-material/fwd.h"
#include "sol-material/graphics/graphics_material2.h"

void Material::operator()()
{
    // Use utility function to create a simple pipeline.
    auto [pipeline, descriptorLayouts] = createSimpleGraphicsPipeline();
    const auto layouts                 = sol::raw(descriptorLayouts);

    // Create material from pipeline and descriptor layouts.
    sol::Material2Ptr material;
    expectNoThrow([&] {
        material =
          std::make_unique<sol::GraphicsMaterial2>(uuids::uuid_system_generator{}(), std::move(pipeline), layouts);
    });

    compareEQ(layouts, material->getDescriptorLayouts());

    compareEQ(&getDevice(), &material->getDevice());
    compareNE(uuids::uuid{}, material->getUuid());
}
