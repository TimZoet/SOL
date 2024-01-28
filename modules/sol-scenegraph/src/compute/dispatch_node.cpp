#include "sol-scenegraph/compute/dispatch_node.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    DispatchNode::DispatchNode(const std::array<uint32_t, 3> value) : count(value) {}

    DispatchNode::DispatchNode(const uint32_t x, const uint32_t y, const uint32_t z) : count{x, y, z} {}

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type DispatchNode::getType() const noexcept { return Type::Dispatch; }

    std::array<uint32_t, 3> DispatchNode::getGroupCount() const noexcept { return count; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void DispatchNode::setGroupCount(const std::array<uint32_t, 3> value) noexcept { count = value; }

    void DispatchNode::setGroupCount(const uint32_t x, const uint32_t y, const uint32_t z) noexcept
    {
        count = {x, y, z};
    }
}  // namespace sol
