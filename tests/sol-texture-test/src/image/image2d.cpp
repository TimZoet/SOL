#include "sol-texture-test/image/image2d.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/image2d2.h"
#include "sol-texture/texture_collection.h"

void Image2D::operator()()
{
    // Create images and verify all properties.

    const auto collection = std::make_unique<sol::TextureCollection>(getMemoryManager());

    sol::Image2D2* image0 = nullptr;
    expectNoThrow([&] {
        image0 = &collection->createImage2D({256, 256},
                                            VK_FORMAT_R8G8B8A8_UINT,
                                            1,
                                            VK_IMAGE_USAGE_SAMPLED_BIT,
                                            VK_IMAGE_ASPECT_COLOR_BIT,
                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                            getMemoryManager().getGraphicsQueue().getFamily(),
                                            VK_IMAGE_TILING_OPTIMAL);
    });

    compareEQ(collection.get(), &image0->getTextureCollection());
    compareEQ(&getMemoryManager().getGraphicsQueue().getFamily(), &image0->getQueueFamily(0, 0));
    expectThrow([&] { static_cast<void>(image0->getQueueFamily(1, 0)); });
    expectThrow([&] { static_cast<void>(image0->getQueueFamily(0, 1)); });
    compareEQ(sol::IImage::ImageType::Image2D, image0->getImageType());
    compareEQ(1, image0->getLevelCount());
    compareEQ(1, image0->getLayerCount());
    compareEQ(std::array<uint32_t, 3>{256, 256, 1}, image0->getSize());
    compareEQ(256, image0->getWidth());
    compareEQ(256, image0->getHeight());
    compareEQ(1, image0->getDepth());
    compareEQ(VK_FORMAT_R8G8B8A8_UINT, image0->getFormat());
    compareEQ(VK_IMAGE_USAGE_SAMPLED_BIT, image0->getImageUsageFlags());
    compareEQ(VK_IMAGE_ASPECT_COLOR_BIT, image0->getImageAspectFlags());
    compareEQ(VK_IMAGE_LAYOUT_UNDEFINED, image0->getImageLayout(0, 0));
    expectThrow([&] { static_cast<void>(image0->getImageLayout(1, 0)); });
    expectThrow([&] { static_cast<void>(image0->getImageLayout(0, 1)); });
    expectThrow([&] { static_cast<void>(image0->getSubresourceLayout(0, 0)); });
    compareEQ(VK_IMAGE_TILING_OPTIMAL, image0->getImageTiling());

    sol::Image2D2* image1 = nullptr;
    expectNoThrow([&] {
        image1 = &collection->createImage2D({1024, 256},
                                            VK_FORMAT_R32G32B32A32_SFLOAT,
                                            4,
                                            VK_IMAGE_USAGE_STORAGE_BIT,
                                            VK_IMAGE_ASPECT_COLOR_BIT,
                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                            getMemoryManager().getTransferQueue().getFamily(),
                                            VK_IMAGE_TILING_LINEAR);
    });

    compareEQ(collection.get(), &image1->getTextureCollection());
    compareEQ(&getMemoryManager().getTransferQueue().getFamily(), &image1->getQueueFamily(0, 0));
    expectThrow([&] { static_cast<void>(image1->getQueueFamily(4, 0)); });
    expectThrow([&] { static_cast<void>(image1->getQueueFamily(0, 1)); });
    compareEQ(sol::IImage::ImageType::Image2D, image1->getImageType());
    compareEQ(4, image1->getLevelCount());
    compareEQ(1, image1->getLayerCount());
    compareEQ(std::array<uint32_t, 3>{1024, 256, 1}, image1->getSize());
    compareEQ(1024, image1->getWidth());
    compareEQ(256, image1->getHeight());
    compareEQ(1, image1->getDepth());
    compareEQ(VK_FORMAT_R32G32B32A32_SFLOAT, image1->getFormat());
    compareEQ(VK_IMAGE_USAGE_STORAGE_BIT, image1->getImageUsageFlags());
    compareEQ(VK_IMAGE_ASPECT_COLOR_BIT, image1->getImageAspectFlags());
    compareEQ(VK_IMAGE_LAYOUT_UNDEFINED, image1->getImageLayout(0, 0));
    expectThrow([&] { static_cast<void>(image1->getImageLayout(4, 0)); });
    expectThrow([&] { static_cast<void>(image1->getImageLayout(0, 1)); });
    expectNoThrow([&] { static_cast<void>(image1->getSubresourceLayout(0, 0)); });
    expectThrow([&] { static_cast<void>(image1->getSubresourceLayout(4, 0)); });
    expectThrow([&] { static_cast<void>(image1->getSubresourceLayout(0, 1)); });
    compareEQ(VK_IMAGE_TILING_LINEAR, image1->getImageTiling());
}
