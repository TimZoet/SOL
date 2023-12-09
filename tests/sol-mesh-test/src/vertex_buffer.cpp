#include "sol-mesh-test/vertex_buffer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction_manager.h"
#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/vertex_buffer.h"

void VertexBuffer::operator()()
{
    constexpr uint32_t elementCount = 12288;
    const auto data = std::views::iota(0) | std::views::take(elementCount) | std::ranges::to<std::vector<uint32_t>>();

    sol::IBufferPtr hostBuffer;
    expectNoThrow([&] {
        const sol::IBufferAllocator::AllocationInfo info{
          .size                 = data.size() * sizeof(uint32_t),
          .bufferUsage          = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags      = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
          .alignment            = 0};
        hostBuffer = getMemoryManager().allocateBuffer(info, sol::IBufferAllocator::OnAllocationFailure::Throw);
    });

    const auto tester = [&](sol::GeometryBufferAllocator* allocator) {
        //
        // Allocate vertex buffers.
        //

        sol::VertexBufferPtr vbuffer0, vbuffer1, vbuffer2;
        expectNoThrow([&] { vbuffer0 = allocator->allocateVertexBuffer(1024, 16); });
        expectNoThrow([&] { vbuffer1 = allocator->allocateVertexBuffer(2048, 16); });
        expectNoThrow([&] { vbuffer2 = allocator->allocateVertexBuffer(2048, 16); });

        //
        // Copy from host to vbuffer0 and vbuffer1.
        //

        const auto t0 = getTransferManager().beginTransaction();

        // Stage copy.
        compareTrue(vbuffer0->setVertexData(*t0,
                                            data.data(),
                                            1024,
                                            0,
                                            sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                                  .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                                                                  .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                                  .srcAccess = VK_ACCESS_2_NONE,
                                                                  .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT},
                                            false));

        // Stage copy.
        compareTrue(vbuffer1->setVertexData(*t0,
                                            data.data() + 4096,
                                            2048,
                                            0,
                                            sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                                  .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                                                                  .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                                  .srcAccess = VK_ACCESS_2_NONE,
                                                                  .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT},
                                            false));

        // Run transaction.
        expectNoThrow([&] {
            t0->commit();
            t0->wait();
        });

        //
        // Copy from vbuffer0 to host to verify contents.
        //

        const auto t1 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            vbuffer0->getData(*t1,
                              *hostBuffer,
                              sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                    .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                    .dstStage  = VK_PIPELINE_STAGE_2_NONE,
                                                    .srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                                    .dstAccess = VK_ACCESS_2_NONE},
                              sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                    .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                                                    .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                    .srcAccess = VK_ACCESS_2_NONE,
                                                    .dstAccess = VK_ACCESS_2_HOST_READ_BIT},
                              vbuffer0->getBufferSize(),
                              0,
                              0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t1->commit();
            t1->wait();
        });

        // Verify contents.
        std::vector<uint32_t> srcData(vbuffer0->getBufferSize() / sizeof(uint32_t));
        std::vector<uint32_t> dstData(vbuffer0->getBufferSize() / sizeof(uint32_t));
        std::memcpy(srcData.data(), data.data(), vbuffer0->getBufferSize());
        std::memcpy(dstData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), vbuffer0->getBufferSize());
        compareEQ(srcData, dstData);

        //
        // Copy from vbuffer1 to host to verify contents.
        //

        const auto t2 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            vbuffer1->getData(*t2,
                              *hostBuffer,
                              sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                    .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                    .dstStage  = VK_PIPELINE_STAGE_2_NONE,
                                                    .srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                                    .dstAccess = VK_ACCESS_2_NONE},
                              sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                    .srcStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                    .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                    .srcAccess = VK_ACCESS_2_HOST_READ_BIT,
                                                    .dstAccess = VK_ACCESS_2_HOST_READ_BIT},
                              vbuffer1->getBufferSize(),
                              0,
                              0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t2->commit();
            t2->wait();
        });

        // Verify contents.
        srcData.resize(vbuffer1->getBufferSize() / sizeof(uint32_t));
        dstData.resize(vbuffer1->getBufferSize() / sizeof(uint32_t));
        std::memcpy(srcData.data(), data.data() + 4096, vbuffer1->getBufferSize());
        std::memcpy(dstData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), vbuffer1->getBufferSize());
        compareEQ(srcData, dstData);

        //
        // Copy from vbuffer1 to vbuffer2.
        //

        const auto t3 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            vbuffer1->getVertexData(*t3,
                                    *vbuffer2,
                                    sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                          .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                          .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                          .srcAccess = VK_ACCESS_2_TRANSFER_READ_BIT,
                                                          .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT},
                                    sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                          .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                                                          .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                          .srcAccess = VK_ACCESS_2_NONE,
                                                          .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT},
                                    vbuffer1->getVertexCount(),
                                    0,
                                    0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t3->commit();
            t3->wait();
        });

        //
        // Copy from vbuffer2 to host to verify contents.
        //

        const auto t4 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            vbuffer2->getData(*t4,
                              *hostBuffer,
                              sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                    .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                                    .dstStage  = VK_PIPELINE_STAGE_2_NONE,
                                                    .srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                                    .dstAccess = VK_ACCESS_2_NONE},
                              sol::IBuffer::Barrier{.dstFamily = nullptr,
                                                    .srcStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                    .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                                                    .srcAccess = VK_ACCESS_2_HOST_READ_BIT,
                                                    .dstAccess = VK_ACCESS_2_HOST_READ_BIT},
                              vbuffer2->getBufferSize(),
                              0,
                              0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t4->commit();
            t4->wait();
        });

        // Verify contents.
        srcData.resize(vbuffer2->getBufferSize() / sizeof(uint32_t));
        dstData.resize(vbuffer2->getBufferSize() / sizeof(uint32_t));
        std::memcpy(srcData.data(), data.data() + 4096, vbuffer2->getBufferSize());
        std::memcpy(dstData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), vbuffer2->getBufferSize());
        compareEQ(srcData, dstData);
    };

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
        tester(allocator.get());
    }

    // Test suballocation from global buffer.
    {
        const sol::GeometryBufferAllocator::Settings settings{.memoryManager = getMemoryManager(),
                                                              .strategy =
                                                                sol::GeometryBufferAllocator::Strategy::Global,
                                                              .vertexCount = 5120,
                                                              .vertexSize  = 16,
                                                              .indexCount  = 5120,
                                                              .indexSize   = 4};

        const auto allocator = sol::GeometryBufferAllocator::create(settings);
        tester(allocator.get());
    }
}
