#include "sol-scenegraph/graphics/graphics_dynamic_state_node.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_dynamic_state.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsDynamicStateNode::GraphicsDynamicStateNode() = default;

    GraphicsDynamicStateNode::GraphicsDynamicStateNode(const uuids::uuid id) : Node(id) {}

    GraphicsDynamicStateNode::~GraphicsDynamicStateNode() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type GraphicsDynamicStateNode::getType() const noexcept { return Type::GraphicsDynamicState; }

    std::vector<GraphicsDynamicStatePtr>& GraphicsDynamicStateNode::getStates() noexcept { return states; }

    const std::vector<GraphicsDynamicStatePtr>& GraphicsDynamicStateNode::getStates() const noexcept { return states; }

}  // namespace sol
