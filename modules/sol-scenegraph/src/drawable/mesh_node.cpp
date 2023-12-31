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

    Node::Type MeshNode::getType() const noexcept { return Type::Mesh; }

    Mesh* MeshNode::getMesh() const noexcept { return mesh; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MeshNode::setMesh(Mesh* m) noexcept { mesh = m; }
}  // namespace sol
