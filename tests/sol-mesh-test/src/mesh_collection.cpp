#include "sol-mesh-test/mesh_collection.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/index_buffer.h"
#include "sol-mesh/mesh_collection.h"
#include "sol-mesh/vertex_buffer.h"

void MeshCollection::operator()()
{
    const sol::GeometryBufferAllocator::Settings settings{.memoryManager = getMemoryManager(),
                                                          .strategy = sol::GeometryBufferAllocator::Strategy::Separate,
                                                          .vertexCount = 0,
                                                          .vertexSize  = 0,
                                                          .indexCount  = 0,
                                                          .indexSize   = 0};

    const auto allocator      = sol::GeometryBufferAllocator::create(settings);
    const auto meshCollection = std::make_unique<sol::MeshCollection>(*allocator);

    sol::Mesh *mesh0, *mesh1, *mesh2, *mesh3;

    // Create a mesh with only a vertex buffer.
    expectNoThrow([&] {
        auto vbuffer = meshCollection->allocateVertexBuffer(1024, 32);
        mesh0        = &meshCollection->createMesh(std::move(vbuffer), {});
    });

    // Create a mesh with a vertex and index buffer.
    expectNoThrow([&] {
        auto vbuffer = meshCollection->allocateVertexBuffer(1024, 32);
        auto ibuffer = meshCollection->allocateIndexBuffer(1024, 4);
        mesh1        = &meshCollection->createMesh(std::move(vbuffer), std::move(ibuffer));
    });

    // Create a mesh with multiple vertex buffers.
    expectNoThrow([&] {
        auto vbuffer0 = meshCollection->allocateVertexBuffer(1024, 32);
        auto vbuffer1 = meshCollection->allocateVertexBuffer(1024, 32);
        mesh2         = &meshCollection->createMesh(std::move(vbuffer0), {}, std::move(vbuffer1));
    });

    // Create a mesh with multiple vertex buffers and an index buffer.
    expectNoThrow([&] {
        auto vbuffer0 = meshCollection->allocateVertexBuffer(1024, 32);
        auto vbuffer1 = meshCollection->allocateVertexBuffer(1024, 32);
        auto vbuffer2 = meshCollection->allocateVertexBuffer(1024, 32);
        auto ibuffer  = meshCollection->allocateIndexBuffer(1024, 4);
        mesh3         = &meshCollection->createMesh(
          std::move(vbuffer0), std::move(ibuffer), std::move(vbuffer1), std::move(vbuffer2));
    });

    const auto meshCollection2 = std::make_unique<sol::MeshCollection>(*allocator);

    // Destroy meshes.
    expectThrow([&] { meshCollection2->destroyMesh(*mesh0); });
    expectNoThrow([&] { meshCollection->destroyMesh(*mesh0); });
    expectNoThrow([&] { meshCollection->destroyMesh(*mesh1); });
    expectNoThrow([&] { meshCollection->destroyMesh(*mesh2); });
    expectNoThrow([&] { meshCollection->destroyMesh(*mesh3); });
}
