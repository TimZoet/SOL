#include "sol-texture/texture2d2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid_system_generator.h>

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

    Texture2D2::Texture2D2(const uuids::uuid id, Image2D2& image2D, Sampler2D& sampler2D, VulkanImageViewPtr view) :
        uuid(id), image(&image2D), sampler(&sampler2D), imageView(std::move(view))
    {
    }

    Texture2D2::Texture2D2(Image2D2& image2D, Sampler2D& sampler2D, VulkanImageViewPtr view) :
        uuid(uuids::uuid_system_generator{}()), image(&image2D), sampler(&sampler2D), imageView(std::move(view))
    {
    }

    Texture2D2::~Texture2D2() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const uuids::uuid& Texture2D2::getUuid() const noexcept { return uuid; }

    Image2D2& Texture2D2::getImage() noexcept { return *image; }

    const Image2D2& Texture2D2::getImage() const noexcept { return *image; }

    Sampler2D& Texture2D2::getSampler() noexcept { return *sampler; }

    const Sampler2D& Texture2D2::getSampler() const noexcept { return *sampler; }

    VulkanImageView& Texture2D2::getImageView() noexcept { return *imageView; }

    const VulkanImageView& Texture2D2::getImageView() const noexcept { return *imageView; }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    Texture2D2Ptr Texture2D2::create(const Settings& settings, uuids::uuid id)
    {
        VulkanImageView::Settings viewSettings;
        viewSettings.image  = settings.image().getImage();
        viewSettings.format = settings.format ? *settings.format : settings.image().getFormat();
        viewSettings.aspect = settings.aspect ? *settings.aspect : settings.image().getImageAspectFlags();
        auto vulkanView     = VulkanImageView::create(viewSettings);

        if (id.is_nil())
            return std::make_unique<Texture2D2>(settings.image(), settings.sampler(), std::move(vulkanView));

        return std::make_unique<Texture2D2>(id, settings.image(), settings.sampler(), std::move(vulkanView));
    }

}  // namespace sol
