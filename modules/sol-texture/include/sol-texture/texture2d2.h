#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>

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
    class Texture2D2
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Texture2D2() = delete;

        Texture2D2(TextureCollection& collection,
                   uuids::uuid        id,
                   Image2D2&          image2D,
                   Sampler2D&         sampler2D,
                   VulkanImageViewPtr view);

        Texture2D2(const Texture2D2&) = delete;

        Texture2D2(Texture2D2&&) noexcept = delete;

        ~Texture2D2() noexcept;

        Texture2D2& operator=(const Texture2D2&) = delete;

        Texture2D2& operator=(Texture2D2&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the texture collection this texture is in.
         * \return TextureCollection.
         */
        [[nodiscard]] TextureCollection& getTextureCollection() noexcept;

        /**
         * \brief Get the texture collection this texture is in.
         * \return TextureCollection.
         */
        [[nodiscard]] const TextureCollection& getTextureCollection() const noexcept;

        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        [[nodiscard]] Image2D2& getImage() noexcept;

        [[nodiscard]] const Image2D2& getImage() const noexcept;

        [[nodiscard]] Sampler2D& getSampler() noexcept;

        [[nodiscard]] const Sampler2D& getSampler() const noexcept;

        [[nodiscard]] VulkanImageView& getImageView() noexcept;

        [[nodiscard]] const VulkanImageView& getImageView() const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Texture collection this sampler is in.
         */
        TextureCollection* textureCollection = nullptr;

        uuids::uuid uuid;

        /**
         * \brief Image.
         */
        Image2D2* image = nullptr;

        /**
         * \brief Sampler.
         */
        Sampler2D* sampler = nullptr;

        /**
         * \brief Image view.
         */
        VulkanImageViewPtr imageView;
    };
}  // namespace sol