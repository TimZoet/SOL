#include "sol-texture-test/image/image2d_barriers.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction.h"
#include "sol-memory/transaction_manager.h"
#include "sol-texture/image2d2.h"

void Image2DBarriers::operator()()
{
    // Copy test data into image.
    sol::Image2D2Ptr image;
    expectNoThrow([&] {
        image = sol::Image2D2::create(sol::Image2D2::Settings{
          .memoryManager = getMemoryManager(),
          .size          = {256u, 256u},
          .format        = VK_FORMAT_R8G8B8A8_UINT,
          .levels        = 4,
          .usage         = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
          .aspect        = VK_IMAGE_ASPECT_COLOR_BIT,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .initialOwner  = getMemoryManager().getGraphicsQueue().getFamily(),
          .tiling        = VK_IMAGE_TILING_OPTIMAL});
    });

    compareEQ(VK_IMAGE_LAYOUT_UNDEFINED, image->getImageLayout(0, 0));
    compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image->getQueueFamily(0, 0));

    // Barrier to compute queue.
    {
        const auto transaction = getTransferManager().beginTransaction();

        image->barrier(*transaction,
                       {.dstFamily = &getMemoryManager().getComputeQueue().getFamily(),
                        .srcStage  = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                        .dstStage  = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                        .srcAccess = VK_ACCESS_2_SHADER_READ_BIT,
                        .dstAccess = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
                        .dstLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL},
                       sol::BarrierLocation::BeforeCopy);

        transaction->commit();
        transaction->wait();
    }

    compareEQ(VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL, image->getImageLayout(0, 0));
    compareEQ(&getMemoryManager().getComputeQueue().getFamily(), &image->getQueueFamily(0, 0));

    // Barrier to transfer queue.
    {
        const auto transaction = getTransferManager().beginTransaction();

        image->barrier(*transaction,
                       {.dstFamily = &getMemoryManager().getTransferQueue().getFamily(),
                        .srcStage  = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                        .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        .srcAccess = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
                        .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT,
                        .dstLayout = VK_IMAGE_LAYOUT_GENERAL},
                       sol::BarrierLocation::BeforeCopy);

        transaction->commit();
        transaction->wait();
    }

    compareEQ(VK_IMAGE_LAYOUT_GENERAL, image->getImageLayout(0, 0));
    compareEQ(&getMemoryManager().getTransferQueue().getFamily(), &image->getQueueFamily(0, 0));

    // Barrier to graphics queue with before and after.
    {
        const auto transaction = getTransferManager().beginTransaction();

        image->barrier(*transaction,
                       {.dstFamily = &getMemoryManager().getGraphicsQueue().getFamily(),
                        .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        .dstStage  = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                        .srcAccess = VK_ACCESS_2_TRANSFER_READ_BIT,
                        .dstAccess = VK_ACCESS_2_SHADER_READ_BIT,
                        .dstLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL},
                       sol::BarrierLocation::BeforeCopy);
        image->barrier(*transaction,
                       {.dstFamily = nullptr,
                        .srcStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                        .dstStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                        .srcAccess = VK_ACCESS_2_SHADER_READ_BIT,
                        .dstAccess = VK_ACCESS_2_SHADER_READ_BIT,
                        .dstLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL},
                       sol::BarrierLocation::AfterCopy);

        transaction->commit();
        transaction->wait();
    }

    compareEQ(VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL, image->getImageLayout(0, 0));
    compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image->getQueueFamily(0, 0));

    // Barrier to compute queue with before and after.
    {
        const auto transaction = getTransferManager().beginTransaction();

        image->barrier(*transaction,
                       {.dstFamily = &getMemoryManager().getTransferQueue().getFamily(),
                        .srcStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                        .dstStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        .srcAccess = VK_ACCESS_2_SHADER_READ_BIT,
                        .dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT,
                        .dstLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL},
                       sol::BarrierLocation::BeforeCopy);
        image->barrier(*transaction,
                       {.dstFamily = &getMemoryManager().getComputeQueue().getFamily(),
                        .srcStage  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                        .dstStage  = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                        .srcAccess = VK_ACCESS_2_TRANSFER_READ_BIT,
                        .dstAccess = VK_ACCESS_2_SHADER_READ_BIT,
                        .dstLayout = VK_IMAGE_LAYOUT_GENERAL},
                       sol::BarrierLocation::AfterCopy);

        transaction->commit();
        transaction->wait();
    }

    compareEQ(VK_IMAGE_LAYOUT_GENERAL, image->getImageLayout(0, 0));
    compareEQ(&getMemoryManager().getComputeQueue().getFamily(), &image->getQueueFamily(0, 0));
}
