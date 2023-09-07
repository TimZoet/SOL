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
    class Sampler2D
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Sampler2D() = delete;

        Sampler2D(TextureCollection& collection, uuids::uuid id, VulkanSamplerPtr s);

        Sampler2D(const Sampler2D&) = delete;

        Sampler2D(Sampler2D&&) = delete;

        ~Sampler2D() noexcept;

        Sampler2D& operator=(const Sampler2D&) = delete;

        Sampler2D& operator=(Sampler2D&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the texture collection this sampler is in.
         * \return TextureCollection.
         */
        [[nodiscard]] TextureCollection& getTextureCollection() noexcept;

        /**
         * \brief Get the texture collection this sampler is in.
         * \return TextureCollection.
         */
        [[nodiscard]] const TextureCollection& getTextureCollection() const noexcept;

        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        [[nodiscard]] VulkanSampler& getSampler() noexcept;

        [[nodiscard]] const VulkanSampler& getSampler() const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Texture collection this sampler is in.
         */
        TextureCollection* textureCollection = nullptr;

        uuids::uuid uuid;

        VulkanSamplerPtr sampler;
    };
}  // namespace sol
