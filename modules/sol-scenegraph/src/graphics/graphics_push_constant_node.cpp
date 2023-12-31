#include "sol-scenegraph/graphics/graphics_push_constant_node.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsPushConstantNode::GraphicsPushConstantNode() = default;

    GraphicsPushConstantNode::GraphicsPushConstantNode(const uuids::uuid id) : Node(id) {}

    GraphicsPushConstantNode::GraphicsPushConstantNode(GraphicsMaterial2& mtl) : material(&mtl) {}

    GraphicsPushConstantNode::GraphicsPushConstantNode(const uuids::uuid id, GraphicsMaterial2& mtl) :
        Node(id), material(&mtl)
    {
    }

    GraphicsPushConstantNode::~GraphicsPushConstantNode() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type GraphicsPushConstantNode::getType() const noexcept { return Type::GraphicsPushConstant; }

    const GraphicsMaterial2* GraphicsPushConstantNode::getMaterial() const noexcept { return material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsPushConstantNode::setMaterial(GraphicsMaterial2* mtl) { material = mtl; }

}  // namespace sol
