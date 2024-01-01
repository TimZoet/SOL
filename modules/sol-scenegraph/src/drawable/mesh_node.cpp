#include "sol-scenegraph/drawable/mesh_node.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MeshNode::MeshNode() = default;

    MeshNode::MeshNode(const uuids::uuid id) : Node(id) {}

    MeshNode::MeshNode(Mesh& m) : mesh(&m) {}

    MeshNode::MeshNode(const uuids::uuid id, Mesh& m) : Node(id), mesh(&m) {}

    MeshNode::~MeshNode() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Mesh* MeshNode::getMesh() const noexcept { return mesh; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MeshNode::setMesh(Mesh* m) noexcept { mesh = m; }

    ////////////////////////////////////////////////////////////////
    // Casting.
    ////////////////////////////////////////////////////////////////

    bool MeshNode::supportsTypeImpl(const Type type) const noexcept { return type == Type::Mesh; }

    void* MeshNode::getAsImpl(const Type type)
    {
        if (type == Type::Mesh) return this;
        return nullptr;
    }
}  // namespace sol
