#include "sol-scenegraph/graphics/graphics_material_node.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardMaterialNode::ForwardMaterialNode(ForwardMaterialInstance& materialInstance) :
        Node(), material(&materialInstance)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type ForwardMaterialNode::getType() const noexcept { return Type::ForwardMaterial; }

    ForwardMaterialInstance* ForwardMaterialNode::getMaterial() const noexcept { return material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ForwardMaterialNode::setMaterial(ForwardMaterialInstance* mtl) noexcept { material = mtl; }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string ForwardMaterialNode::getVizLabel() const { return "ForwardMaterial"; }

    std::string ForwardMaterialNode::getVizShape() const { return "circle"; }

    std::string ForwardMaterialNode::getVizFillColor() const { return material ? "white" : "red"; }
}  // namespace sol
