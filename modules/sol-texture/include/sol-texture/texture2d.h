#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

namespace sol
{
    class Texture2D
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Texture2D();

        /**
         * \brief Construct a texture without initializing the image and image view. Constructs a default sampler.
         * \param manager Texture manager.
         */
        explicit Texture2D(TextureManager& manager);

        /**
         * \brief Construct a texture with an image. Constructs a default image view and sampler.
         * \param manager Texture manager.
         * \param img Image.
         */
        Texture2D(TextureManager& manager, Image2D& img);

        /**
         * \brief Construct a texture without initializing the image and image view.
         * \param manager Texture manager.
         * \param imgSampler Sampler. If null, a default sampler is constructed.
         */
        Texture2D(TextureManager& manager, VulkanSamplerPtr imgSampler);

        /**
         * \brief Construct a texture with an image and optionally an image view and sampler.
         * \param manager Texture manager.
         * \param img Image.
         * \param imgView Image view. If null, a default image view is constructed.
         * \param imgSampler Sampler. If null, a default sampler is constructed.
         */
        Texture2D(TextureManager& manager, Image2D& img, VulkanImageViewPtr imgView, VulkanSamplerPtr imgSampler);

        Texture2D(const Texture2D&) = delete;

        Texture2D(Texture2D&&) = delete;

        ~Texture2D() noexcept;

        Texture2D& operator=(const Texture2D&) = delete;

        Texture2D& operator=(Texture2D&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] TextureManager& getTextureManager() noexcept;

        [[nodiscard]] const TextureManager& getTextureManager() const noexcept;

        /**
         * \brief Get the image.
         * \return Image2D or null.
         */
        [[nodiscard]] Image2D* getImage() const noexcept;

        /**
         * \brief Get the image view object.
         * \return VulkanImageView or null.
         */
        [[nodiscard]] VulkanImageView* getImageView() noexcept;

        /**
         * \brief Get the image view object.
         * \return VulkanImageView or null.
         */
        [[nodiscard]] const VulkanImageView* getImageView() const noexcept;

        /**
         * \brief Get the sampler object.
         * \return VulkanSampler.
         */
        [[nodiscard]] const VulkanSampler& getSampler() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set image. Will automatically recreate VulkanImageView.
         * \param img Image2D.
         */
        void setImage(Image2D& img);

        /**
         * \brief Set image view.
         * \param imgView VulkanImageView. Must refer to the same VulkanImage as the Image2D of this texture.
         */
        void setImageView(VulkanImageViewPtr imgView);

        /**
         * \brief Set image and view.
         * \param img Image2D.
         * \param imgView VulkanImageView. Must refer to the same VulkanImage as the Image2D of this texture.
         */
        void setImageAndView(Image2D& img, VulkanImageViewPtr imgView);

        /**
         * \brief Clear image and image view.
         */
        void clearImage();

        /**
         * \brief Set the sampler.
         * \param imgSampler VulkanSampler.
         */
        void setSampler(VulkanSamplerPtr imgSampler);

    private:
        ////////////////////////////////////////////////////////////////
        // Default create methods.
        ////////////////////////////////////////////////////////////////

        void createDefaultImageView();

        void createDefaultSampler();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Manager of this texture.
         */
        TextureManager* textureManager = nullptr;

        /**
         * \brief Image.
         */
        Image2D* image = nullptr;

        /**
         * \brief Image view. Can be null if image is not set.
         */
        VulkanImageViewPtr imageView;

        /**
         * \brief Sampler. Is never null.
         */
        VulkanSamplerPtr sampler;
    };
}  // namespace sol