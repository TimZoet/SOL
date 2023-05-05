#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

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
    class VulkanSwapchain
    {
        struct CreateUtils
        {
            VkSwapchainKHR     swapchain  = VK_NULL_HANDLE;
            uint32_t           imageCount = 0;
            VkSurfaceFormatKHR surfaceFormat{};
            VkPresentModeKHR   presentMode = VK_PRESENT_MODE_FIFO_KHR;
            VkExtent2D         extent{0, 0};
        };

    public:
        struct Settings
        {
            /**
             * \brief Vulkan surface.
             */
            ObjectRefSetting<VulkanSurface> surface;

            /**
             * \brief Vulkan physical device.
             */
            ObjectRefSetting<VulkanPhysicalDevice> physicalDevice;

            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Preferred present mode.
             */
            VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;

            /**
             * \brief Extent.
             */
            VkExtent2D extent;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanSwapchain() = delete;

        VulkanSwapchain(SettingsPtr settingsPtr, VkSwapchainKHR vkSwapchain, CreateUtils createUtils);

        VulkanSwapchain(const VulkanSwapchain&) = delete;

        VulkanSwapchain(VulkanSwapchain&&) = delete;

        ~VulkanSwapchain() noexcept;

        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

        VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan swapchain.
         * \param settings Settings.
         * \throws VulkanError Thrown if swapchain creation failed.
         * \return Swapchain.
         */
        [[nodiscard]] static VulkanSwapchainPtr create(Settings settings);

        /**
         * \brief Create a new Vulkan swapchain.
         * \param settings Settings.
         * \throws VulkanError Thrown if swapchain creation failed.
         * \return Swapchain.
         */
        [[nodiscard]] static VulkanSwapchainSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

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
         * \brief Get the swapchain handle managed by this object.
         * \return Swapchain handle.
         */
        [[nodiscard]] const VkSwapchainKHR& get() const noexcept;

        /**
         * \brief Get the surface format.
         * \return Surface format.
         */
        [[nodiscard]] const VkSurfaceFormatKHR& getSurfaceFormat() const noexcept;

        /**
         * \brief Get the present mode.
         * \return Present mode
         */
        [[nodiscard]] const VkPresentModeKHR& getPresentMode() const noexcept;

        /**
         * \brief Get the extent.
         * \return Extent.
         */
        [[nodiscard]] const VkExtent2D& getExtent() const noexcept;

        /**
         * \brief Get the number of images.
         * \return Image count.
         */
        [[nodiscard]] size_t getImageCount() const noexcept;

        /**
         * \brief Get the image list.
         * \return Images.
         */
        [[nodiscard]] const std::vector<VulkanImagePtr>& getImages() const noexcept;

        /**
         * \brief Get the image view list.
         * \return Image views.
         */
        [[nodiscard]] const std::vector<VulkanImageViewPtr>& getImageViews() const noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void recreate();

    private:
        [[nodiscard]] static VkSwapchainKHR createImpl(const Settings& settings, CreateUtils& createUtils);

        [[nodiscard]] static VkSurfaceFormatKHR selectSurfaceFormat(const Settings& settings);

        [[nodiscard]] static VkPresentModeKHR selectPresentMode(const Settings& settings);

        [[nodiscard]] static VkExtent2D selectExtent(const Settings& settings);

        void retrieveImages();

        void createImageViews();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan swap chain.
         */
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        /**
         * \brief Surface format.
         */
        VkSurfaceFormatKHR surfaceFormat;

        /**
         * \brief Present mode.
         */
        VkPresentModeKHR presentMode;

        /**
         * \brief Extent.
         */
        VkExtent2D extent;

        /**
         * \brief List of images.
         */
        std::vector<VulkanImagePtr> images;

        /**
         * \brief List of image views.
         */
        std::vector<VulkanImageViewPtr> imageViews;
    };
}  // namespace sol