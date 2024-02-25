#include "sol-mesh/mesh.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#define NOMINMAX
#include "uuid_system_generator.h"

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

    Mesh::Mesh() : uuid(uuids::uuid_system_generator{}()) {}

    Mesh::Mesh(VertexBufferPtr vb0) : uuid(uuids::uuid_system_generator{}())
    {
        vertexBuffers.emplace_back(std::move(vb0));
    }

    Mesh::Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1) : uuid(uuids::uuid_system_generator{}())
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
    }

    Mesh::Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2) : uuid(uuids::uuid_system_generator{}())
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
        vertexBuffers.emplace_back(std::move(vb2));
    }

    Mesh::Mesh(VertexBufferPtr vb0, IndexBufferPtr ib) : uuid(uuids::uuid_system_generator{}())
    {
        vertexBuffers.emplace_back(std::move(vb0));
        indexBuffer = std::move(ib);
    }

    Mesh::Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1, IndexBufferPtr ib) : uuid(uuids::uuid_system_generator{}())
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
        indexBuffer = std::move(ib);
    }

    Mesh::Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2, IndexBufferPtr ib) :
        uuid(uuids::uuid_system_generator{}())
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
        vertexBuffers.emplace_back(std::move(vb2));
        indexBuffer = std::move(ib);
    }

    Mesh::Mesh(const uuids::uuid id) : uuid(id) {}

    Mesh::Mesh(const uuids::uuid id, VertexBufferPtr vb0) : uuid(id) { vertexBuffers.emplace_back(std::move(vb0)); }

    Mesh::Mesh(const uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1) : uuid(id)
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
    }

    Mesh::Mesh(const uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2) : uuid(id)
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
        vertexBuffers.emplace_back(std::move(vb2));
    }

    Mesh::Mesh(const uuids::uuid id, VertexBufferPtr vb0, IndexBufferPtr ib) : uuid(id)
    {
        vertexBuffers.emplace_back(std::move(vb0));
        indexBuffer = std::move(ib);
    }

    Mesh::Mesh(const uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1, IndexBufferPtr ib) : uuid(id)
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
        indexBuffer = std::move(ib);
    }

    Mesh::Mesh(const uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2, IndexBufferPtr ib) :
        uuid(id)
    {
        vertexBuffers.emplace_back(std::move(vb0));
        vertexBuffers.emplace_back(std::move(vb1));
        vertexBuffers.emplace_back(std::move(vb2));
        indexBuffer = std::move(ib);
    }

    Mesh::~Mesh() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const uuids::uuid& Mesh::getUuid() const noexcept { return uuid; }

    size_t Mesh::getVertexBufferCount() const noexcept { return vertexBuffers.size(); }

    bool Mesh::hasIndexBuffer() const noexcept { return indexBuffer != nullptr; }

    std::vector<VertexBufferPtr>& Mesh::getVertexBuffers() noexcept { return vertexBuffers; }

    const std::vector<VertexBufferPtr>& Mesh::getVertexBuffers() const noexcept { return vertexBuffers; }

    IndexBufferPtr& Mesh::getIndexBuffer() noexcept { return indexBuffer; }

    const IndexBufferPtr& Mesh::getIndexBuffer() const noexcept { return indexBuffer; }
}  // namespace sol
