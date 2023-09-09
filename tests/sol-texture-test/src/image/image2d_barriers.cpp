#include "sol-texture-test/image/image2d_barriers.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-memory/buffer_transaction.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transfer_manager.h"
#include "sol-texture/image2d2.h"
#include "sol-texture/texture_collection.h"

void Image2DBarriers::operator()()
{
    const auto collection = std::make_unique<sol::TextureCollection>(getMemoryManager());

    sol::Image2D2* image = nullptr;
    expectNoThrow([&] {
        image = &collection->createImage2D({256, 256},
                                           VK_FORMAT_R8G8B8A8_UINT,
                                           1,
                                           VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                           VK_IMAGE_ASPECT_COLOR_BIT,
                                           VK_IMAGE_LAYOUT_UNDEFINED,
                                           getMemoryManager().getGraphicsQueue().getFamily(),
                                           VK_IMAGE_TILING_OPTIMAL);
    });

    const auto data = genR8G8B8A8W256H256Gradient();

    const auto transaction = getTransferManager().beginTransaction();

    constexpr sol::Image2D2::CopyRegion region{
      .dataOffset = 0, .level = 0, .regionOffset = {0, 0}, .regionSize = {256, 256}};
    compareTrue(image->setData(*transaction,
                               data.data(),
                               data.size() * 4,
                               {.dstFamily = nullptr,
                                .srcStage  = VK_PIPELINE_STAGE_2_NONE,
                                .dstStage  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                .srcAccess = VK_ACCESS_2_NONE,
                                .dstAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                                .dstLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
                               false,
                               {region}));

    transaction->commit();
    transaction->wait();
}
