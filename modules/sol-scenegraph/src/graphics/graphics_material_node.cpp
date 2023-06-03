#include "sol-scenegraph/graphics/graphics_material_node.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsMaterialNode::GraphicsMaterialNode(GraphicsMaterialInstance& materialInstance) :
        Node(), material(&materialInstance)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type GraphicsMaterialNode::getType() const noexcept { return Type::GraphicsMaterial; }

    GraphicsMaterialInstance* GraphicsMaterialNode::getMaterial() const noexcept { return material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialNode::setMaterial(GraphicsMaterialInstance* mtl) noexcept { material = mtl; }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string GraphicsMaterialNode::getVizLabel() const { return "GraphicsMaterial"; }

    std::string GraphicsMaterialNode::getVizShape() const { return "circle"; }

    std::string GraphicsMaterialNode::getVizFillColor() const { return material ? "white" : "red"; }
}  // namespace sol
