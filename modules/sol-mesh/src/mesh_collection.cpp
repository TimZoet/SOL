#include "sol-mesh/mesh_collection.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

// TODO: This should not be necessary and fixed by the uuid lib.
#define NOMINMAX
#include <uuid_system_generator.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/index_buffer.h"
#include "sol-mesh/mesh.h"
#include "sol-mesh/vertex_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MeshCollection::MeshCollection(GeometryBufferAllocator& alloc) : allocator(&alloc) {}

    MeshCollection::~MeshCollection() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    GeometryBufferAllocator& MeshCollection::getAllocator() noexcept { return *allocator; }

    const GeometryBufferAllocator& MeshCollection::getAllocator() const noexcept { return *allocator; }

    ////////////////////////////////////////////////////////////////
    // Meshes.
    ////////////////////////////////////////////////////////////////

    VertexBufferPtr MeshCollection::allocateVertexBuffer(const size_t count, const size_t size) const
    {
        return allocator->allocateVertexBuffer(count, size);
    }

    IndexBufferPtr MeshCollection::allocateIndexBuffer(const size_t count, const size_t size) const
    {
        return allocator->allocateIndexBuffer(count, size);
    }

    Mesh& MeshCollection::createMeshImpl()
    {
        auto mesh = std::make_unique<Mesh>(*this, uuids::uuid_system_generator{}());
        return *meshes.emplace(mesh->getUuid(), std::move(mesh)).first->second;
    }

    void MeshCollection::destroyMesh(const Mesh& mesh)
    {
        if (&mesh.getMeshCollection() != this)
            throw SolError("Cannot destroy mesh that is part of a different mesh collection.");
        const auto it = meshes.find(mesh.getUuid());
        if (it == meshes.end()) throw SolError("Cannot destroy mesh: mesh was already destroyed.");
        meshes.erase(it);
    }
}  // namespace sol
