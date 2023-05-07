#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

namespace sol
{
    class VulkanImageView
    {
    public:
        /**
         * \brief VulkanImageView settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan image.
             */
            ObjectRefSetting<VulkanImage> image;

            /**
             * \brief Format.
             */
            VkFormat format = VK_FORMAT_UNDEFINED;

            /**
             * \brief Image aspect.
             */
            VkImageAspectFlags aspect = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;

            VkComponentMapping components = {VK_COMPONENT_SWIZZLE_IDENTITY,
                                             VK_COMPONENT_SWIZZLE_IDENTITY,
                                             VK_COMPONENT_SWIZZLE_IDENTITY,
                                             VK_COMPONENT_SWIZZLE_IDENTITY};
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanImageView() = delete;

        VulkanImageView(const Settings& set, VkImageView vkImageView);

        VulkanImageView(const VulkanImageView&) = delete;

        VulkanImageView(VulkanImageView&&) = delete;

        ~VulkanImageView() noexcept;

        VulkanImageView& operator=(const VulkanImageView&) = delete;

        VulkanImageView& operator=(VulkanImageView&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan image view.
         * \param settings Settings.
         * \throws VulkanError Thrown if image view creation failed.
         * \return Vulkan image view.
         */
        [[nodiscard]] static VulkanImageViewPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan image view.
         * \param settings Settings.
         * \throws VulkanError Thrown if image view creation failed.
         * \return Vulkan image view.
         */
        [[nodiscard]] static VulkanImageViewSharedPtr createShared(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;
#endif

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        /**
         * \brief Get the image view handle managed by this object.
         * \return Image view handle.
         */
        [[nodiscard]] const VkImageView& get() const noexcept;

        /**
         * \brief Get the image.
         * \return VulkanImage.
         */
        [[nodiscard]] VulkanImage& getImage() noexcept;

        /**
         * \brief Get the image.
         * \return VulkanImage.
         */
        [[nodiscard]] const VulkanImage& getImage() const noexcept;

    private:
        [[nodiscard]] static VkImageView createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanImage* image = nullptr;
#endif

        /**
         * \brief Vulkan image view.
         */
        VkImageView imageView = VK_NULL_HANDLE;
    };
}  // namespace sol