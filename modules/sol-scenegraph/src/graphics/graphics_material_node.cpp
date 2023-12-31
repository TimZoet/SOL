#include "sol-scenegraph/graphics/graphics_material_node.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsMaterialNode::GraphicsMaterialNode() = default;

    GraphicsMaterialNode::GraphicsMaterialNode(const uuids::uuid id) : Node(id) {}

    GraphicsMaterialNode::GraphicsMaterialNode(GraphicsMaterialInstance2& m) : material(&m) {}

    GraphicsMaterialNode::GraphicsMaterialNode(const uuids::uuid id, GraphicsMaterialInstance2& m) :
        Node(id), material(&m)
    {
    }

    GraphicsMaterialNode::~GraphicsMaterialNode() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type GraphicsMaterialNode::getType() const noexcept { return Type::GraphicsMaterial; }

    GraphicsMaterialInstance2* GraphicsMaterialNode::getMaterial() const noexcept { return material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialNode::setMaterial(GraphicsMaterialInstance2* mtl) noexcept { material = mtl; }
}  // namespace sol
