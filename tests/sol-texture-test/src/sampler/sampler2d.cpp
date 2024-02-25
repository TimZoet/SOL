#include "sol-texture-test/sampler/sampler2d.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid_system_generator.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/sampler2d.h"

void Sampler2D::operator()()
{
    // Create samplers and verify all properties.

    sol::Sampler2DPtr sampler0;
    expectNoThrow([&] { sampler0 = sol::Sampler2D::create(sol::Sampler2D::Settings{.device = getDevice()}); });
    compareNE(uuids::uuid{}, sampler0->getUuid());
    expectNoThrow([&] { static_cast<void>(sampler0->getSampler()); });
    // TODO: Test properties of samplers.

    const auto        id = uuids::uuid_system_generator{}();
    sol::Sampler2DPtr sampler1;
    expectNoThrow([&] {
        sampler1 =
          sol::Sampler2D::create(sol::Sampler2D::Settings{.device       = getDevice(),
                                                          .magFilter    = VK_FILTER_NEAREST,
                                                          .minFilter    = VK_FILTER_LINEAR,
                                                          .mipmapMode   = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                                                          .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                          .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                          .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER},
                                 id);
    });
    compareEQ(id, sampler1->getUuid());
    expectNoThrow([&] { static_cast<void>(sampler1->getSampler()); });
    // TODO: Test properties of samplers.
}
