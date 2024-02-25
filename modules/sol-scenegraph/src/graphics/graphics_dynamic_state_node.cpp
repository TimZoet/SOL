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

    std::vector<GraphicsDynamicStatePtr>& GraphicsDynamicStateNode::getStates() noexcept { return states; }

    const std::vector<GraphicsDynamicStatePtr>& GraphicsDynamicStateNode::getStates() const noexcept { return states; }

    ////////////////////////////////////////////////////////////////
    // Casting.
    ////////////////////////////////////////////////////////////////

    bool GraphicsDynamicStateNode::supportsTypeImpl(const Type type) const noexcept
    {
        return type == Type::GraphicsDynamicState;
    }

    const void* GraphicsDynamicStateNode::getAsImpl(const Type type) const
    {
        if (type == Type::GraphicsDynamicState) return this;
        return nullptr;
    }

}  // namespace sol
