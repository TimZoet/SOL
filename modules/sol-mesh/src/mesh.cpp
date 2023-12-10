#include "sol-mesh/mesh.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/index_buffer.h"
#include "sol-mesh/vertex_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Mesh::Mesh() = default;

    Mesh::Mesh(MeshCollection& collection, const uuids::uuid id) : meshCollection(&collection), uuid(id) {}

    Mesh::~Mesh() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MeshCollection& Mesh::getMeshCollection() noexcept { return *meshCollection; }

    const MeshCollection& Mesh::getMeshCollection() const noexcept { return *meshCollection; }

    const uuids::uuid& Mesh::getUuid() const noexcept { return uuid; }

    size_t Mesh::getVertexBufferCount() const noexcept { return vertexBuffers.size(); }

    bool Mesh::hasIndexBuffer() const noexcept { return indexBuffer != nullptr; }

    const std::vector<VertexBufferPtr>& Mesh::getVertexBuffers() const noexcept { return vertexBuffers; }

    const IndexBufferPtr& Mesh::getIndexBuffer() const noexcept { return indexBuffer; }
}  // namespace sol
