#include "pathtracing/textures.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/image2d.h"
#include "sol-texture/texture_manager.h"

sol::Texture2D& createMissingTexture(sol::TextureManager& manager)
{
    auto& missingImage = manager.createImage2D(VK_FORMAT_R32G32B32A32_SFLOAT, {256, 256});
    missingImage.createStagingBuffer();
    const std::array white = {1.0f, 1.0f, 1.0f, 1.0f};
    const std::array black = {0.0f, 0.0f, 0.0f, 1.0f};

    constexpr uint32_t blocks    = 32;
    const uint32_t     blockSize = missingImage.getWidth() / blocks;
    for (uint32_t y = 0; y < blocks / 2; y++)
        for (uint32_t x = 0; x < blocks / 2; x++)
        {
            missingImage.fill(white.data(), {x * blockSize * 2, y * blockSize * 2}, {blockSize, blockSize}, 0);
            missingImage.fill(
              black.data(), {x * blockSize * 2 + blockSize, y * blockSize * 2}, {blockSize, blockSize}, 0);
            missingImage.fill(
              black.data(), {x * blockSize * 2, y * blockSize * 2 + blockSize}, {blockSize, blockSize}, 0);
            missingImage.fill(
              white.data(), {x * blockSize * 2 + blockSize, y * blockSize * 2 + blockSize}, {blockSize, blockSize}, 0);
        }

    return manager.createTexture2D(missingImage);
}

sol::Texture2D& createFontTexture(GlobalState& state)
{
    unsigned char* pixels;
    int            width, height;
    state.imgui->Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    auto& fontImage = state.guiTextureManager->createImage2D(
      VK_FORMAT_R8G8B8A8_UNORM, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}, pixels);
    return state.guiTextureManager->createTexture2D(fontImage);
}
