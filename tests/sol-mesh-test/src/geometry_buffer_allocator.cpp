#include "sol-mesh-test/geometry_buffer_allocator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/index_buffer.h"
#include "sol-mesh/vertex_buffer.h"

void GeometryBufferAllocator::operator()()
{
    // Test allocation with separate buffers.
    {
        const sol::GeometryBufferAllocator::Settings settings{.memoryManager = getMemoryManager(),
                                                              .strategy =
                                                                sol::GeometryBufferAllocator::Strategy::Separate,
                                                              .vertexCount = 0,
                                                              .vertexSize  = 0,
                                                              .indexCount  = 0,
                                                              .indexSize   = 0};

        const auto allocator = sol::GeometryBufferAllocator::create(settings);

        compareEQ(sol::IBufferAllocator::Capabilities::None, allocator->getCapabilities());
        compareEQ(sol::GeometryBufferAllocator::Strategy::Separate, allocator->getStrategy());
        compareEQ(nullptr, allocator->getVirtualVertexBlock());
        compareEQ(nullptr, allocator->getVirtualIndexBlock());

        // Allocate a free vertex buffer.
        sol::VertexBufferPtr vbuffer;
        expectNoThrow([&] { vbuffer = allocator->allocateVertexBuffer(1024, 16); });
        compareEQ(allocator.get(), &vbuffer->getAllocator());
        compareEQ(1024, vbuffer->getVertexCount());
        compareEQ(16, vbuffer->getVertexSize());
        compareEQ(1024 * 16, vbuffer->getBufferSize());
        compareEQ(0, vbuffer->getBufferOffset());
        compareFalse(vbuffer->isSubAllocation());
        compareEQ(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  vbuffer->getBuffer().getSettings().bufferUsage);

        // Allocate with invalid parameters.
        expectThrow([&] { static_cast<void>(allocator->allocateVertexBuffer(0, 16)); });
        expectThrow([&] { static_cast<void>(allocator->allocateVertexBuffer(1024, 0)); });

        // Allocate a free index buffer.
        sol::IndexBufferPtr ibuffer;
        expectNoThrow([&] { ibuffer = allocator->allocateIndexBuffer(2048, 4); });
        compareEQ(2048, ibuffer->getIndexCount());
        compareEQ(4, ibuffer->getIndexSize());
        compareEQ(2048 * 4, ibuffer->getBufferSize());
        compareEQ(0, ibuffer->getBufferOffset());
        compareFalse(ibuffer->isSubAllocation());
        compareEQ(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  ibuffer->getBuffer().getSettings().bufferUsage);

        // Allocate with invalid parameters.
        expectThrow([&] { static_cast<void>(allocator->allocateIndexBuffer(0, 4)); });
        expectThrow([&] { static_cast<void>(allocator->allocateIndexBuffer(2048, 0)); });
    }

    // Test suballocation from global buffer.
    {
        const sol::GeometryBufferAllocator::Settings settings{.memoryManager = getMemoryManager(),
                                                              .strategy =
                                                                sol::GeometryBufferAllocator::Strategy::Global,
                                                              .vertexCount = 1024,
                                                              .vertexSize  = 16,
                                                              .indexCount  = 2048,
                                                              .indexSize   = 4};

        const auto allocator = sol::GeometryBufferAllocator::create(settings);

        compareEQ(sol::IBufferAllocator::Capabilities::None, allocator->getCapabilities());
        compareEQ(sol::GeometryBufferAllocator::Strategy::Global, allocator->getStrategy());
        compareNE(nullptr, allocator->getVirtualVertexBlock());
        compareNE(nullptr, allocator->getVirtualIndexBlock());

        // Try to allocate some vertex buffers.
        sol::VertexBufferPtr vbuffer0, vbuffer1, vbuffer2;
        expectNoThrow([&] { vbuffer0 = allocator->allocateVertexBuffer(512); });
        compareEQ(512, vbuffer0->getVertexCount());
        compareEQ(16, vbuffer0->getVertexSize());
        compareEQ(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  vbuffer0->getBuffer().getSettings().bufferUsage);
        expectNoThrow([&] { vbuffer1 = allocator->allocateVertexBuffer(256); });
        compareEQ(256, vbuffer1->getVertexCount());
        compareEQ(16, vbuffer1->getVertexSize());
        expectNoThrow([&] { vbuffer2 = allocator->allocateVertexBuffer(128); });
        compareEQ(128, vbuffer2->getVertexCount());
        compareEQ(16, vbuffer2->getVertexSize());

        // Check they all share the same underlying buffer.
        compareEQ(&vbuffer0->getBuffer(), &vbuffer1->getBuffer());
        compareEQ(&vbuffer1->getBuffer(), &vbuffer2->getBuffer());
        compareEQ(1024 * 16, vbuffer0->getBuffer().getSize());

        // This one should not fit anymore.
        expectThrow([&] { static_cast<void>(allocator->allocateVertexBuffer(256)); });

        // Free up space and try again.
        vbuffer0.reset();
        expectNoThrow([&] { static_cast<void>(allocator->allocateVertexBuffer(256)); });

        // Try to allocate some index buffers.
        sol::IndexBufferPtr ibuffer0, ibuffer1, ibuffer2;
        expectNoThrow([&] { ibuffer0 = allocator->allocateIndexBuffer(1024); });
        compareEQ(1024, ibuffer0->getIndexCount());
        compareEQ(4, ibuffer0->getIndexSize());
        compareEQ(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  ibuffer0->getBuffer().getSettings().bufferUsage);
        expectNoThrow([&] { ibuffer1 = allocator->allocateIndexBuffer(512); });
        compareEQ(512, ibuffer1->getIndexCount());
        compareEQ(4, ibuffer1->getIndexSize());
        expectNoThrow([&] { ibuffer2 = allocator->allocateIndexBuffer(256); });
        compareEQ(256, ibuffer2->getIndexCount());
        compareEQ(4, ibuffer2->getIndexSize());

        // Check they all share the same underlying buffer.
        compareEQ(&ibuffer0->getBuffer(), &ibuffer1->getBuffer());
        compareEQ(&ibuffer1->getBuffer(), &ibuffer2->getBuffer());
        compareEQ(2048 * 4, ibuffer0->getBuffer().getSize());

        // This one should not fit anymore.
        expectThrow([&] { static_cast<void>(allocator->allocateIndexBuffer(512)); });

        // Free up space and try again.
        ibuffer0.reset();
        expectNoThrow([&] { static_cast<void>(allocator->allocateIndexBuffer(512)); });
    }

    // Create allocator with invalid global settings.
    {
        sol::GeometryBufferAllocator::Settings settings{.memoryManager = getMemoryManager(),
                                                        .strategy      = sol::GeometryBufferAllocator::Strategy::Global,
                                                        .vertexCount   = 1024,
                                                        .vertexSize    = 16,
                                                        .indexCount    = 2048,
                                                        .indexSize     = 4};
        settings.vertexCount = 0;
        expectThrow([&] { static_cast<void>(sol::GeometryBufferAllocator::create(settings)); });
        settings.vertexCount = 1024;
        settings.vertexSize  = 0;
        expectThrow([&] { static_cast<void>(sol::GeometryBufferAllocator::create(settings)); });
        settings.vertexSize = 16;
        settings.indexCount = 0;
        expectThrow([&] { static_cast<void>(sol::GeometryBufferAllocator::create(settings)); });
        settings.indexCount = 2048;
        settings.indexSize  = 0;
        expectThrow([&] { static_cast<void>(sol::GeometryBufferAllocator::create(settings)); });
    }
}
