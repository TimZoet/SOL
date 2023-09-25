#include "sol-texture-test/sampler/sampler2d.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/sampler2d.h"
#include "sol-texture/texture_collection.h"


void Sampler2D::operator()()
{
    // Create samplers and verify all properties.

    const auto collection = std::make_unique<sol::TextureCollection>(getMemoryManager());

    sol::Sampler2D* sampler0 = nullptr;
    expectNoThrow([&] { sampler0 = &collection->createSampler2D(); });
    compareEQ(collection.get(), &sampler0->getTextureCollection());
    compareNE(uuids::uuid{}, sampler0->getUuid());
    expectNoThrow([&] { static_cast<void>(sampler0->getSampler()); });
    // TODO: Test properties of samplers.

    sol::Sampler2D* sampler1 = nullptr;
    expectNoThrow([&] {
        sampler1 = &collection->createSampler2D(VK_FILTER_NEAREST,
                                                VK_FILTER_LINEAR,
                                                VK_SAMPLER_MIPMAP_MODE_NEAREST,
                                                VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
    });
    compareEQ(collection.get(), &sampler1->getTextureCollection());
    compareNE(uuids::uuid{}, sampler1->getUuid());
    expectNoThrow([&] { static_cast<void>(sampler1->getSampler()); });
    // TODO: Test properties of samplers.

    expectNoThrow([&] { collection->destroySampler(*sampler0); });
    expectNoThrow([&] { collection->destroySampler(*sampler1); });
}
