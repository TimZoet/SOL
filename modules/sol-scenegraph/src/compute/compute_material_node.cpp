#include "sol-scenegraph/compute/compute_material_node.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialNode::ComputeMaterialNode(ComputeMaterialInstance& materialInstance) : material(&materialInstance) {}

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type ComputeMaterialNode::getType() const noexcept { return Type::ComputeMaterial; }

    ComputeMaterialInstance* ComputeMaterialNode::getMaterial() const noexcept { return material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ComputeMaterialNode::setMaterial(ComputeMaterialInstance* mtl) noexcept { material = mtl; }
}  // namespace sol
