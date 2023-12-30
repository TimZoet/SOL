#include "sol-texture-test/image/image2d.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/image2d2.h"

void Image2D::operator()()
{
    // Create images and verify all properties.

    sol::Image2D2Ptr image0;
    expectNoThrow([&] {
        image0 = sol::Image2D2::create(
          sol::Image2D2::Settings{.memoryManager = getMemoryManager(),
                                  .size          = {256u, 256u},
                                  .format        = VK_FORMAT_R8G8B8A8_UINT,
                                  .levels        = 4,
                                  .usage         = VK_IMAGE_USAGE_SAMPLED_BIT,
                                  .aspect        = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                  .initialOwner  = getMemoryManager().getGraphicsQueue().getFamily(),
                                  .tiling        = VK_IMAGE_TILING_OPTIMAL});
    });

    compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image0->getQueueFamily(0, 0));
    expectThrow([&] { static_cast<void>(image0->getQueueFamily(4, 0)); });
    expectThrow([&] { static_cast<void>(image0->getQueueFamily(0, 1)); });
    compareEQ(sol::IImage::ImageType::Image2D, image0->getImageType());
    compareEQ(4, image0->getLevelCount());
    compareEQ(1, image0->getLayerCount());
    compareEQ(std::array<uint32_t, 3>{256, 256, 1}, image0->getSize());
    compareEQ(256, image0->getWidth());
    compareEQ(256, image0->getHeight());
    compareEQ(1, image0->getDepth());
    compareEQ(VK_FORMAT_R8G8B8A8_UINT, image0->getFormat());
    compareEQ(VK_IMAGE_USAGE_SAMPLED_BIT, image0->getImageUsageFlags());
    compareEQ(VK_IMAGE_ASPECT_COLOR_BIT, image0->getImageAspectFlags());
    compareEQ(VK_IMAGE_LAYOUT_UNDEFINED, image0->getImageLayout(0, 0));
    expectThrow([&] { static_cast<void>(image0->getImageLayout(4, 0)); });
    expectThrow([&] { static_cast<void>(image0->getImageLayout(0, 1)); });
    expectThrow([&] { static_cast<void>(image0->getSubresourceLayout(0, 0)); });
    compareEQ(VK_IMAGE_TILING_OPTIMAL, image0->getImageTiling());

    sol::Image2D2Ptr image1;
    expectNoThrow([&] {
        image1 = sol::Image2D2::create(
          sol::Image2D2::Settings{.memoryManager = getMemoryManager(),
                                  .size          = {1024u, 256u},
                                  .format        = VK_FORMAT_R32G32B32A32_SFLOAT,
                                  .levels        = 1,
                                  .usage         = VK_IMAGE_USAGE_STORAGE_BIT,
                                  .aspect        = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                  .initialOwner  = getMemoryManager().getTransferQueue().getFamily(),
                                  .tiling        = VK_IMAGE_TILING_LINEAR});
    });

    compareEQ(&getMemoryManager().getTransferQueue().getFamily(), &image1->getQueueFamily(0, 0));
    expectThrow([&] { static_cast<void>(image1->getQueueFamily(1, 0)); });
    expectThrow([&] { static_cast<void>(image1->getQueueFamily(0, 1)); });
    compareEQ(sol::IImage::ImageType::Image2D, image1->getImageType());
    compareEQ(1, image1->getLevelCount());
    compareEQ(1, image1->getLayerCount());
    compareEQ(std::array<uint32_t, 3>{1024, 256, 1}, image1->getSize());
    compareEQ(1024, image1->getWidth());
    compareEQ(256, image1->getHeight());
    compareEQ(1, image1->getDepth());
    compareEQ(VK_FORMAT_R32G32B32A32_SFLOAT, image1->getFormat());
    compareEQ(VK_IMAGE_USAGE_STORAGE_BIT, image1->getImageUsageFlags());
    compareEQ(VK_IMAGE_ASPECT_COLOR_BIT, image1->getImageAspectFlags());
    compareEQ(VK_IMAGE_LAYOUT_UNDEFINED, image1->getImageLayout(0, 0));
    expectThrow([&] { static_cast<void>(image1->getImageLayout(1, 0)); });
    expectThrow([&] { static_cast<void>(image1->getImageLayout(0, 1)); });
    expectNoThrow([&] { static_cast<void>(image1->getSubresourceLayout(0, 0)); });
    expectThrow([&] { static_cast<void>(image1->getSubresourceLayout(1, 0)); });
    expectThrow([&] { static_cast<void>(image1->getSubresourceLayout(0, 1)); });
    compareEQ(VK_IMAGE_TILING_LINEAR, image1->getImageTiling());
}
