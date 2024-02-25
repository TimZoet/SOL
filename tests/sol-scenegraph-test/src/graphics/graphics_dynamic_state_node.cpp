#include "sol-scenegraph-test/graphics/graphics_dynamic_state_node.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid_system_generator.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_dynamic_state.h"
#include "sol-scenegraph/graphics/graphics_dynamic_state_node.h"

void GraphicsDynamicStateNode::operator()()
{
    /*
     * Test all constructors.
     */

    {
        const auto node = std::make_unique<sol::GraphicsDynamicStateNode>();
        compareNE(uuids::uuid{}, node->getUuid());
        compareTrue(node->getStates().empty());
    }

    {
        const auto id   = uuids::uuid_system_generator{}();
        const auto node = std::make_unique<sol::GraphicsDynamicStateNode>(id);
        compareEQ(id, node->getUuid());
        compareTrue(node->getStates().empty());
    }

    /*
     * Test remaining methods.
     */

    {
        const auto node = std::make_unique<sol::GraphicsDynamicStateNode>();
        compareTrue(node->supportsType(sol::Node::Type::Empty));
        compareTrue(node->supportsType(sol::Node::Type::GraphicsDynamicState));
        compareEQ(node.get(), node->getAs(sol::Node::Type::Empty));
        compareEQ(node.get(), node->getAs(sol::Node::Type::GraphicsDynamicState));
        compareTrue(node->getStates().empty());
        expectNoThrow([&] { node->getStates().push_back(std::make_unique<sol::CullMode>()); });
        expectNoThrow([&] { node->getStates().push_back(std::make_unique<sol::Viewport>()); });
        expectNoThrow([&] { node->getStates().clear(); });
    }
}
