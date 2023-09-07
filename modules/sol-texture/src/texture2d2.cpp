#include "sol-texture/texture2d2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/image2d.h"
#include "sol-texture/image2d2.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Texture2D2::Texture2D2(TextureCollection& collection,
                           const uuids::uuid  id,
                           Image2D2&          image2D,
                           Sampler2D&         sampler2D,
                           VulkanImageViewPtr view) :
        textureCollection(&collection), uuid(id), image(&image2D), sampler(&sampler2D), imageView(std::move(view))
    {
    }

    Texture2D2::~Texture2D2() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    TextureCollection& Texture2D2::getTextureCollection() noexcept { return *textureCollection; }

    const TextureCollection& Texture2D2::getTextureCollection() const noexcept { return *textureCollection; }

    const uuids::uuid& Texture2D2::getUuid() const noexcept { return uuid; }

    Image2D2& Texture2D2::getImage() noexcept { return *image; }

    const Image2D2& Texture2D2::getImage() const noexcept { return *image; }

    Sampler2D& Texture2D2::getSampler() noexcept { return *sampler; }

    const Sampler2D& Texture2D2::getSampler() const noexcept { return *sampler; }

    VulkanImageView& Texture2D2::getImageView() noexcept { return *imageView; }

    const VulkanImageView& Texture2D2::getImageView() const noexcept { return *imageView; }
}  // namespace sol
