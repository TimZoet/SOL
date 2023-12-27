#include "sol-material-test/graphics/graphics_material_instance.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/descriptor_layout.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-material/graphics/graphics_material_instance2.h"

void GraphicsMaterialInstance::operator()()
{
    // Use utility function to create a simple pipeline.
    auto [pipeline, descriptorLayouts] = createSimpleGraphicsPipeline();
    const auto layouts = descriptorLayouts | std::views::transform([](const auto& v) { return v.get(); }) |
                         std::ranges::to<std::vector<const sol::DescriptorLayout*>>();

    // Create material from pipeline and descriptor layouts.
    sol::GraphicsMaterial2Ptr material;
    expectNoThrow([&] { material = std::make_unique<sol::GraphicsMaterial2>(std::move(pipeline), layouts); });

    // Create material instance.
    sol::GraphicsMaterialInstance2Ptr instance;
    expectNoThrow([&] { instance = std::make_unique<sol::GraphicsMaterialInstance2>(*material); });
    compareEQ(material.get(), &instance->getGraphicsMaterial());
}
