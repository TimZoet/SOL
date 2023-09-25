#include "sol-texture-test/texture/texture2d.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/sampler2d.h"
#include "sol-texture/texture2d2.h"
#include "sol-texture/texture_collection.h"


void Texture2D::operator()()
{
    const auto collection0 = std::make_unique<sol::TextureCollection>(getMemoryManager());
    const auto collection1 = std::make_unique<sol::TextureCollection>(getMemoryManager());

    sol::Image2D2*  image0   = nullptr;
    sol::Image2D2*  image1   = nullptr;
    sol::Sampler2D* sampler0 = nullptr;
    sol::Sampler2D* sampler1 = nullptr;
    expectNoThrow([&] {
        image0   = &collection0->createImage2D({512, 512},
                                             VK_FORMAT_R8G8B8A8_SRGB,
                                             1,
                                             VK_IMAGE_USAGE_SAMPLED_BIT,
                                             VK_IMAGE_ASPECT_COLOR_BIT,
                                             VK_IMAGE_LAYOUT_UNDEFINED,
                                             getMemoryManager().getGraphicsQueue().getFamily(),
                                             VK_IMAGE_TILING_OPTIMAL);
        image1   = &collection1->createImage2D(*image0);
        sampler0 = &collection0->createSampler2D();
        sampler1 = &collection1->createSampler2D();
    });

    // Create some valid textures.
    const sol::Texture2D2* texture0 = nullptr;
    const sol::Texture2D2* texture1 = nullptr;
    expectNoThrow([&] { texture0 = &collection0->createTexture2D(*image0, *sampler0); });
    expectNoThrow([&] { texture1 = &collection1->createTexture2D(*image1, *sampler1); });

    compareEQ(collection0.get(), &texture0->getTextureCollection());
    compareEQ(collection1.get(), &texture1->getTextureCollection());
    compareNE(uuids::uuid{}, texture0->getUuid());
    compareNE(uuids::uuid{}, texture1->getUuid());
    compareEQ(image0, &texture0->getImage());
    compareEQ(image1, &texture1->getImage());
    compareEQ(sampler0, &texture0->getSampler());
    compareEQ(sampler1, &texture1->getSampler());
    expectNoThrow([&] { static_cast<void>(texture0->getSampler()); });
    expectNoThrow([&] { static_cast<void>(texture1->getSampler()); });

    // Try to create textures with images and samplers from different collections.
    expectThrow([&] { static_cast<void>(collection0->createTexture2D(*image0, *sampler1)); });
    expectThrow([&] { static_cast<void>(collection0->createTexture2D(*image1, *sampler0)); });
    expectThrow([&] { static_cast<void>(collection0->createTexture2D(*image1, *sampler1)); });

    // Destroy textures.
    expectNoThrow([&] { collection0->destroyTexture(*texture0); });
    expectThrow([&] { collection0->destroyTexture(*texture1); });
    expectNoThrow([&] { collection1->destroyTexture(*texture1); });
}
