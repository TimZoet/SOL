#include "sol-material-test/graphics/graphics_material_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-material/graphics/graphics_material_instance2.h"

void GraphicsMaterialInstance::operator()()
{
    // Use utility function to create a simple pipeline.
    auto [pipeline, descriptorLayouts] = createSimpleGraphicsPipeline();
    const auto layouts = sol::raw(descriptorLayouts);

    // Create material from pipeline and descriptor layouts.
    sol::GraphicsMaterial2Ptr material;
    expectNoThrow([&] { material = std::make_unique<sol::GraphicsMaterial2>(std::move(pipeline), layouts); });

    // Create material instance.
    sol::GraphicsMaterialInstance2Ptr instance;
    expectNoThrow([&] { instance = material->createInstance<sol::GraphicsMaterialInstance2>(); });
    compareEQ(material.get(), &instance->getGraphicsMaterial());
}
