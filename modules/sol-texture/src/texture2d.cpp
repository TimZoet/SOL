#include "sol-texture/texture2d.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-error/sol_error.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/image2d.h"
#include "sol-texture/texture_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Texture2D::Texture2D() = default;

    Texture2D::Texture2D(TextureManager& manager) : textureManager(&manager) { createDefaultSampler(); }

    Texture2D::Texture2D(TextureManager& manager, Image2D& img) : textureManager(&manager), image(&img)
    {
        createDefaultImageView();
        createDefaultSampler();
    }

    Texture2D::Texture2D(TextureManager& manager, VulkanSamplerPtr imgSampler) :
        textureManager(&manager), sampler(std::move(imgSampler))
    {
        if (!sampler) createDefaultSampler();
    }

    Texture2D::Texture2D(TextureManager&    manager,
                         Image2D&           img,
                         VulkanImageViewPtr imgView,
                         VulkanSamplerPtr   imgSampler) :
        textureManager(&manager), image(&img), imageView(std::move(imgView)), sampler(std::move(imgSampler))
    {
        if (!imageView) createDefaultImageView();
        if (!sampler) createDefaultSampler();

        assert(&imageView->getImage() == &image->getImage());
    }

    Texture2D::~Texture2D() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    TextureManager& Texture2D::getTextureManager() noexcept { return *textureManager; }

    const TextureManager& Texture2D::getTextureManager() const noexcept { return *textureManager; }

    Image2D* Texture2D::getImage() const noexcept { return image; }

    VulkanImageView* Texture2D::getImageView() noexcept { return imageView.get(); }

    const VulkanImageView* Texture2D::getImageView() const noexcept { return imageView.get(); }

    const VulkanSampler& Texture2D::getSampler() const noexcept { return *sampler; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Texture2D::setImage(Image2D& img)
    {
        if (image != &img)
        {
            image = &img;

            // Recreate view with same settings but new image.
            auto settings  = imageView->getSettings();
            settings.image = image->getImage();
            imageView      = VulkanImageView::create(settings);
        }
    }

    void Texture2D::setImageView(VulkanImageViewPtr imgView)
    {
        if (!imgView) throw SolError("Image view cannot be null.");
        if (&image->getImage() != &imgView->getImage())
            throw SolError("Image2D and VulkanImageView do not refer to the same VulkanImage.");

        imageView = std::move(imgView);
    }

    void Texture2D::setImageAndView(Image2D& img, VulkanImageViewPtr imgView)
    {
        if (!imgView) throw SolError("Image view cannot be null.");
        if (&img.getImage() != &imgView->getImage())
            throw SolError("Image2D and VulkanImageView do not refer to the same VulkanImage.");

        image     = &img;
        imageView = std::move(imgView);
    }

    void Texture2D::clearImage()
    {
        if (image)
        {
            image = nullptr;
            imageView.reset();
        }
    }

    void Texture2D::setSampler(VulkanSamplerPtr imgSampler)
    {
        if (!imgSampler) throw SolError("Sampler cannot be null.");
        sampler = std::move(imgSampler);
    }

    ////////////////////////////////////////////////////////////////
    // Default create methods.
    ////////////////////////////////////////////////////////////////

    void Texture2D::createDefaultImageView()
    {
        VulkanImageView::Settings imageViewSettings;
        imageViewSettings.image  = image->getImage();
        imageViewSettings.format = image->getFormat();
        imageViewSettings.aspect = image->getAspectFlags();
        imageView                = VulkanImageView::create(imageViewSettings);
    }

    void Texture2D::createDefaultSampler()
    {
        VulkanSampler::Settings settings;
        settings.device = textureManager->getMemoryManager().getDevice();
        sampler         = VulkanSampler::create(settings);
    }
}  // namespace sol
