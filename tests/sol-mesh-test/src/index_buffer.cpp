#include "sol-mesh-test/index_buffer.h"

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
#include "sol-mesh/index_buffer.h"

void IndexBuffer::operator()()
{
    constexpr uint32_t elementCount = 3072;
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
        // Allocate index buffers.
        //

        sol::IndexBufferPtr ibuffer0, ibuffer1, ibuffer2;
        expectNoThrow([&] { ibuffer0 = allocator->allocateIndexBuffer(1024, 4); });
        expectNoThrow([&] { ibuffer1 = allocator->allocateIndexBuffer(2048, 4); });
        expectNoThrow([&] { ibuffer2 = allocator->allocateIndexBuffer(2048, 4); });

        //
        // Copy from host to ibuffer0 and ibuffer1.
        //

        const auto t0 = getTransferManager().beginTransaction();

        // Stage copy.
        compareTrue(ibuffer0->setIndexData(*t0,
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
        compareTrue(ibuffer1->setIndexData(*t0,
                                           data.data() + 1024,
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
        // Copy from ibuffer0 to host to verify contents.
        //

        const auto t1 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            ibuffer0->getData(*t1,
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
                              ibuffer0->getBufferSize(),
                              0,
                              0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t1->commit();
            t1->wait();
        });

        // Verify contents.
        std::vector<uint32_t> srcData(ibuffer0->getBufferSize() / sizeof(uint32_t));
        std::vector<uint32_t> dstData(ibuffer0->getBufferSize() / sizeof(uint32_t));
        std::memcpy(srcData.data(), data.data(), ibuffer0->getBufferSize());
        std::memcpy(dstData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), ibuffer0->getBufferSize());
        compareEQ(srcData, dstData);

        //
        // Copy from ibuffer1 to host to verify contents.
        //

        const auto t2 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            ibuffer1->getData(*t2,
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
                              ibuffer1->getBufferSize(),
                              0,
                              0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t2->commit();
            t2->wait();
        });

        // Verify contents.
        srcData.resize(ibuffer1->getBufferSize() / sizeof(uint32_t));
        dstData.resize(ibuffer1->getBufferSize() / sizeof(uint32_t));
        std::memcpy(srcData.data(), data.data() + 1024, ibuffer1->getBufferSize());
        std::memcpy(dstData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), ibuffer1->getBufferSize());
        compareEQ(srcData, dstData);

        //
        // Copy from ibuffer1 to ibuffer2.
        //

        const auto t3 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            ibuffer1->getIndexData(*t3,
                                   *ibuffer2,
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
                                   ibuffer1->getIndexCount(),
                                   0,
                                   0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t3->commit();
            t3->wait();
        });

        //
        // Copy from ibuffer2 to host to verify contents.
        //

        const auto t4 = getTransferManager().beginTransaction();

        // Stage copy.
        expectNoThrow([&] {
            ibuffer2->getData(*t4,
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
                              ibuffer2->getBufferSize(),
                              0,
                              0);
        });

        // Run transaction.
        expectNoThrow([&] {
            t4->commit();
            t4->wait();
        });

        // Verify contents.
        srcData.resize(ibuffer2->getBufferSize() / sizeof(uint32_t));
        dstData.resize(ibuffer2->getBufferSize() / sizeof(uint32_t));
        std::memcpy(srcData.data(), data.data() + 1024, ibuffer2->getBufferSize());
        std::memcpy(dstData.data(), hostBuffer->getBuffer().getMappedData<uint32_t>(), ibuffer2->getBufferSize());
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
