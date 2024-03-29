#include "sol-texture-test/image/image2d_data.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_queue.h"
#include "sol-memory/i_buffer.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction.h"
#include "sol-memory/transaction_manager.h"
#include "sol-texture/image2d2.h"

void Image2DData::operator()()
{
    // Generate some test data.
    const auto data = genR8G8B8A8W256H256Gradient();

    // Create a 256x256 image and a 512x512 image.
    sol::Image2D2Ptr image0;
    sol::Image2D2Ptr image1;
    expectNoThrow([&] {
        image0 = sol::Image2D2::create(sol::Image2D2::Settings{
          .memoryManager = getMemoryManager(),
          .size          = {256u, 256u},
          .format        = VK_FORMAT_R8G8B8A8_UINT,
          .levels        = 1,
          .usage  = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
          .aspect = VK_IMAGE_ASPECT_COLOR_BIT,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .initialOwner  = getMemoryManager().getGraphicsQueue().getFamily(),
          .tiling        = VK_IMAGE_TILING_OPTIMAL});

        image1 = sol::Image2D2::create(sol::Image2D2::Settings{
          .memoryManager = getMemoryManager(),
          .size          = {512u, 512u},
          .format        = VK_FORMAT_R8G8B8A8_UINT,
          .levels        = 4,
          .usage  = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
          .aspect = VK_IMAGE_ASPECT_COLOR_BIT,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .initialOwner  = getMemoryManager().getGraphicsQueue().getFamily(),
          .tiling        = VK_IMAGE_TILING_OPTIMAL});
    });

    // Copy test data into images.
    {
        const auto transaction = getTransferManager().beginTransaction();

        constexpr sol::Image2D2::CopyRegion region0{
          .dataOffset = 0, .level = 0, .regionOffset = {0, 0}, .regionSize = {256, 256}};
        constexpr sol::Image2D2::CopyRegion region1{
          .dataOffset = 0, .level = 0, .regionOffset = {256, 0}, .regionSize = {256, 256}};
        constexpr sol::Image2D2::CopyRegion region2{
          .dataOffset = 0, .level = 0, .regionOffset = {0, 256}, .regionSize = {256, 256}};
        constexpr sol::Image2D2::CopyRegion region3{
          .dataOffset = 0, .level = 0, .regionOffset = {256, 256}, .regionSize = {256, 256}};

        // Test data fits directly into image0.
        compareTrue(image0->setData(*transaction,
                                    data.data(),
                                    data.size() * 4,
                                    {.dstFamily = nullptr,
                                     .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                                     .dstStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                     .srcAccess = VK_ACCESS_2_NONE,
                                     .dstAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                                     .dstLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL},
                                    false,
                                    {region0}));

        // Test data is put into image1 as 4 blocks.
        compareTrue(image1->setData(*transaction,
                                    data.data(),
                                    data.size() * 4,
                                    {.dstFamily = nullptr,
                                     .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                                     .dstStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                     .srcAccess = VK_ACCESS_2_NONE,
                                     .dstAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                                     .dstLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL},
                                    false,
                                    {region0, region1, region2, region3}));

        transaction->commit();
        transaction->wait();
    }

    // Verify layout and queue family.
    compareEQ(VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL, image0->getImageLayout(0, 0));
    compareEQ(VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL, image1->getImageLayout(0, 0));
    compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image0->getQueueFamily(0, 0));
    compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image1->getQueueFamily(0, 0));

    {
        // Create a host-side buffer to copy the image data back to.
        sol::IBufferAllocator::AllocationInfo alloc{.size                 = data.size() * 4,
                                                    .bufferUsage          = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                    .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
                                                    .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                    .requiredMemoryFlags  = 0,
                                                    .preferredMemoryFlags = 0,
                                                    .allocationFlags      = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                                                       VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                                                    .alignment = 0};
        const auto                            buffer0 =
          getMemoryManager().allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);
        alloc.size *= 4;
        const auto buffer1 =
          getMemoryManager().allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);

        // Copy data back.
        const auto                          transaction = getTransferManager().beginTransaction();
        constexpr sol::Image2D2::CopyRegion region0{
          .dataOffset = 0, .level = 0, .regionOffset = {0, 0}, .regionSize = {256, 256}};
        constexpr sol::Image2D2::CopyRegion region1{
          .dataOffset = 0, .level = 0, .regionOffset = {0, 0}, .regionSize = {512, 512}};
        image0->getData(*transaction,
                        *buffer0,
                        {.dstFamily = nullptr,
                         .srcStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                         .dstStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                         .srcAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                         .dstAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                         .dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
                        {.dstFamily = nullptr,
                         .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                         .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                         .srcAccess = VK_ACCESS_2_NONE,
                         .dstAccess = VK_ACCESS_2_HOST_READ_BIT,
                         .dstLayout = VK_IMAGE_LAYOUT_UNDEFINED},
                        {region0});
        image1->getData(*transaction,
                        *buffer1,
                        {.dstFamily = nullptr,
                         .srcStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                         .dstStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                         .srcAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                         .dstAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                         .dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
                        {.dstFamily = nullptr,
                         .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                         .dstStage  = VK_PIPELINE_STAGE_2_HOST_BIT,
                         .srcAccess = VK_ACCESS_2_NONE,
                         .dstAccess = VK_ACCESS_2_HOST_READ_BIT,
                         .dstLayout = VK_IMAGE_LAYOUT_UNDEFINED},
                        {region1});
        transaction->commit();
        transaction->wait();

        // Verify layout and queue family.
        compareEQ(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image0->getImageLayout(0, 0));
        compareEQ(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image1->getImageLayout(0, 0));
        compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image0->getQueueFamily(0, 0));
        compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image1->getQueueFamily(0, 0));
        compareEQ(&getMemoryManager().getTransferQueue().getFamily(), &buffer0->getQueueFamily());
        compareEQ(&getMemoryManager().getTransferQueue().getFamily(), &buffer0->getQueueFamily());

        // Get buffer data into vector and compare.

        std::vector<uint32_t> dataCopy(256ull * 256ull, 0);
        std::memcpy(dataCopy.data(), buffer0->getBuffer().getMappedData<uint32_t>(), 256ull * 256ull * 4);
        compareEQ(data, dataCopy);

        for (size_t y = 0; y < 256; y++)
            std::memcpy(
              dataCopy.data() + y * 256, buffer1->getBuffer().getMappedData<uint32_t>() + y * 512, 256ull * 4);
        compareEQ(data, dataCopy);

        for (size_t y = 0; y < 256; y++)
            std::memcpy(
              dataCopy.data() + y * 256, buffer1->getBuffer().getMappedData<uint32_t>() + y * 512 + 256, 256ull * 4);
        compareEQ(data, dataCopy);

        for (size_t y = 0; y < 256; y++)
            std::memcpy(dataCopy.data() + y * 256,
                        buffer1->getBuffer().getMappedData<uint32_t>() + y * 512 + 256ull * 512,
                        256ull * 4);
        compareEQ(data, dataCopy);

        for (size_t y = 0; y < 256; y++)
            std::memcpy(dataCopy.data() + y * 256,
                        buffer1->getBuffer().getMappedData<uint32_t>() + y * 512 + 256ull * 512 + 256ull,
                        256ull * 4);
        compareEQ(data, dataCopy);
    }
}
