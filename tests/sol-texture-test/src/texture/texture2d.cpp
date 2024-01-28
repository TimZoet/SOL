#include "sol-texture-test/texture/texture2d.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid_system_generator.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/image2d2.h"
#include "sol-texture/sampler2d.h"
#include "sol-texture/texture2d2.h"

void Texture2D::operator()()
{
    sol::Image2D2Ptr  image0;
    sol::Image2D2Ptr  image1;
    sol::Sampler2DPtr sampler0;
    sol::Sampler2DPtr sampler1;
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
        image1 = sol::Image2D2::create(
          sol::Image2D2::Settings{.memoryManager = getMemoryManager(),
                                  .size          = {256u, 256u},
                                  .format        = VK_FORMAT_R8G8B8A8_UINT,
                                  .levels        = 4,
                                  .usage         = VK_IMAGE_USAGE_SAMPLED_BIT,
                                  .aspect        = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                  .initialOwner  = getMemoryManager().getGraphicsQueue().getFamily(),
                                  .tiling        = VK_IMAGE_TILING_OPTIMAL});
        sampler0 = sol::Sampler2D::create(sol::Sampler2D::Settings{.device = getDevice()});
        sampler1 = sol::Sampler2D::create(sol::Sampler2D::Settings{.device = getDevice()});
    });

    // Create some valid textures.
    const auto         id = uuids::uuid_system_generator{}();
    sol::Texture2D2Ptr texture0;
    sol::Texture2D2Ptr texture1;
    expectNoThrow(
      [&] { texture0 = sol::Texture2D2::create(sol::Texture2D2::Settings{.image = *image0, .sampler = *sampler0}); });
    expectNoThrow([&] {
        texture1 = sol::Texture2D2::create(sol::Texture2D2::Settings{.image = *image1, .sampler = *sampler1}, id);
    });

    compareNE(uuids::uuid{}, texture0->getUuid());
    compareEQ(id, texture1->getUuid());
    compareEQ(image0.get(), &texture0->getImage());
    compareEQ(image1.get(), &texture1->getImage());
    compareEQ(sampler0.get(), &texture0->getSampler());
    compareEQ(sampler1.get(), &texture1->getSampler());
    expectNoThrow([&] { static_cast<void>(texture0->getSampler()); });
    expectNoThrow([&] { static_cast<void>(texture1->getSampler()); });
}
