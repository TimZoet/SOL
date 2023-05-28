#include "sol-scenegraph/ray_tracing/ray_tracing_material_node.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterialNode::RayTracingMaterialNode(RayTracingMaterialInstance& materialInstance) :
        material(&materialInstance)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type RayTracingMaterialNode::getType() const noexcept { return Type::RayTracingMaterial; }

    RayTracingMaterialInstance* RayTracingMaterialNode::getMaterial() const noexcept { return material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RayTracingMaterialNode::setMaterial(RayTracingMaterialInstance* mtl) noexcept { material = mtl; }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string RayTracingMaterialNode::getVizLabel() const { return "RayTracingMaterial"; }

    std::string RayTracingMaterialNode::getVizShape() const { return "circle"; }

    std::string RayTracingMaterialNode::getVizFillColor() const { return material ? "white" : "red"; }
}  // namespace sol
