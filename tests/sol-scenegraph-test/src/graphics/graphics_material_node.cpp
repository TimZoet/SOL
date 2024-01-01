#include "sol-scenegraph-test/graphics/graphics_material_node.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_graphics_pipeline2.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-scenegraph/graphics/graphics_material_node.h"

void GraphicsMaterialNode::operator()()
{
    // Use utility function to create a simple material instance.
    auto [pipeline, descriptorLayouts] = createSimpleGraphicsPipeline();
    const auto layouts                 = sol::raw(descriptorLayouts);
    const auto material =
      std::make_unique<sol::GraphicsMaterial2>(uuids::uuid_system_generator{}(), std::move(pipeline), layouts);
    const auto instance = material->createInstance();

    /*
     * Test all constructors.
     */

    {
        const auto node = std::make_unique<sol::GraphicsMaterialNode>();
        compareNE(uuids::uuid{}, node->getUuid());
        compareEQ(nullptr, node->getMaterial());
    }

    {
        const auto id   = uuids::uuid_system_generator{}();
        const auto node = std::make_unique<sol::GraphicsMaterialNode>(id);
        compareEQ(id, node->getUuid());
        compareEQ(nullptr, node->getMaterial());
    }

    {
        const auto id   = uuids::uuid_system_generator{}();
        const auto node = std::make_unique<sol::GraphicsMaterialNode>(id, *instance);
        compareEQ(id, node->getUuid());
        compareEQ(instance.get(), node->getMaterial());
    }

    {
        const auto node = std::make_unique<sol::GraphicsMaterialNode>(*instance);
        compareNE(uuids::uuid{}, node->getUuid());
        compareEQ(instance.get(), node->getMaterial());
    }

    /*
     * Test remaining methods.
     */

    {
        const auto node = std::make_unique<sol::GraphicsMaterialNode>();
        compareTrue(node->supportsType(sol::Node::Type::Empty));
        compareTrue(node->supportsType(sol::Node::Type::GraphicsMaterial));
        compareEQ(node.get(), node->getAs(sol::Node::Type::Empty));
        compareEQ(node.get(), node->getAs(sol::Node::Type::GraphicsMaterial));
        compareEQ(nullptr, node->getMaterial());
        expectNoThrow([&] { node->setMaterial(instance.get()); });
        compareEQ(instance.get(), node->getMaterial());
    }
}
