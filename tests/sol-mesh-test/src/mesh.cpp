#include "sol-mesh-test/mesh.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/index_buffer.h"
#include "sol-mesh/mesh.h"
#include "sol-mesh/vertex_buffer.h"

void Mesh::operator()()
{
    const sol::GeometryBufferAllocator::Settings settings{.memoryManager = getMemoryManager(),
                                                          .strategy = sol::GeometryBufferAllocator::Strategy::Separate,
                                                          .vertexCount = 0,
                                                          .vertexSize  = 0,
                                                          .indexCount  = 0,
                                                          .indexSize   = 0};

    const auto allocator = sol::GeometryBufferAllocator::create(settings);

    sol::MeshPtr mesh0, mesh1, mesh2, mesh3;

    // Create a mesh with only a vertex buffer.
    expectNoThrow([&] {
        auto vbuffer = allocator->allocateVertexBuffer(1024, 32);
        mesh0        = std::make_unique<sol::Mesh>(std::move(vbuffer));
    });
    compareNE(uuids::uuid{}, mesh0->getUuid());
    compareEQ(1, mesh0->getVertexBufferCount());
    compareFalse(mesh0->hasIndexBuffer());

    // Create a mesh with a vertex and index buffer.
    expectNoThrow([&] {
        auto vbuffer = allocator->allocateVertexBuffer(1024, 32);
        auto ibuffer = allocator->allocateIndexBuffer(1024, 4);
        mesh1        = std::make_unique<sol::Mesh>(std::move(vbuffer), std::move(ibuffer));
    });
    compareNE(uuids::uuid{}, mesh1->getUuid());
    compareEQ(1, mesh1->getVertexBufferCount());
    compareTrue(mesh1->hasIndexBuffer());

    // Create a mesh with multiple vertex buffers.
    expectNoThrow([&] {
        auto vbuffer0 = allocator->allocateVertexBuffer(1024, 32);
        auto vbuffer1 = allocator->allocateVertexBuffer(1024, 32);
        mesh2         = std::make_unique<sol::Mesh>(std::move(vbuffer0), std::move(vbuffer1));
    });
    compareNE(uuids::uuid{}, mesh2->getUuid());
    compareEQ(2, mesh2->getVertexBufferCount());
    compareFalse(mesh2->hasIndexBuffer());

    // Create a mesh with multiple vertex buffers and an index buffer.
    expectNoThrow([&] {
        auto vbuffer0 = allocator->allocateVertexBuffer(1024, 32);
        auto vbuffer1 = allocator->allocateVertexBuffer(1024, 32);
        auto vbuffer2 = allocator->allocateVertexBuffer(1024, 32);
        auto ibuffer  = allocator->allocateIndexBuffer(1024, 4);
        mesh3         = std::make_unique<sol::Mesh>(
          std::move(vbuffer0), std::move(vbuffer1), std::move(vbuffer2), std::move(ibuffer));
    });
    compareNE(uuids::uuid{}, mesh3->getUuid());
    compareEQ(3, mesh3->getVertexBufferCount());
    compareTrue(mesh3->hasIndexBuffer());
}
