#pragma once
////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

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
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Settings
        {
            ObjectRefSetting<Image2D2> image;

            ObjectRefSetting<Sampler2D> sampler;

            /**
             * \brief Image format. Set to image.getFormat() if left undefined.
             */
            std::optional<VkFormat> format = {};

            /**
             * \brief Image aspect. Set to image.getImageAspectFlags() if left undefined.
             */
            std::optional<VkImageAspectFlags> aspect = {};

            VkComponentMapping components = {VK_COMPONENT_SWIZZLE_IDENTITY,
                                             VK_COMPONENT_SWIZZLE_IDENTITY,
                                             VK_COMPONENT_SWIZZLE_IDENTITY,
                                             VK_COMPONENT_SWIZZLE_IDENTITY};
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Texture2D2() = delete;

        Texture2D2(uuids::uuid id, Image2D2& image2D, Sampler2D& sampler2D, VulkanImageViewPtr view);

        Texture2D2(Image2D2& image2D, Sampler2D& sampler2D, VulkanImageViewPtr view);

        Texture2D2(const Texture2D2&) = delete;

        Texture2D2(Texture2D2&&) noexcept = delete;

        ~Texture2D2() noexcept;

        Texture2D2& operator=(const Texture2D2&) = delete;

        Texture2D2& operator=(Texture2D2&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        [[nodiscard]] Image2D2& getImage() noexcept;

        [[nodiscard]] const Image2D2& getImage() const noexcept;

        [[nodiscard]] Sampler2D& getSampler() noexcept;

        [[nodiscard]] const Sampler2D& getSampler() const noexcept;

        [[nodiscard]] VulkanImageView& getImageView() noexcept;

        [[nodiscard]] const VulkanImageView& getImageView() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new 2D texture.
         * \param settings Settings.
         * \param id Identifier. If empty, generated automatically.
         * \return New Texture2D.
         */
        [[nodiscard]] static Texture2D2Ptr create(const Settings& settings, uuids::uuid id = uuids::uuid{});

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

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