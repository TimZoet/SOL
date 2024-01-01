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

    GraphicsMaterialInstance2* GraphicsMaterialNode::getMaterial() const noexcept { return material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialNode::setMaterial(GraphicsMaterialInstance2* mtl) noexcept { material = mtl; }

    ////////////////////////////////////////////////////////////////
    // Casting.
    ////////////////////////////////////////////////////////////////

    bool GraphicsMaterialNode::supportsTypeImpl(const Type type) const noexcept { return type == Type::GraphicsMaterial; }

    void* GraphicsMaterialNode::getAsImpl(const Type type)
    {
        if (type == Type::GraphicsMaterial) return this;
        return nullptr;
    }
}  // namespace sol
