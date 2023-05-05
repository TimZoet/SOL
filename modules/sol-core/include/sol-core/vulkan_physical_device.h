#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
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
#include "sol-core/vulkan_queue_family.h"
#include "sol-core/vulkan_swapchain_support_details.h"

namespace sol
{
    /**
     * \brief The VulkanInstance object manages the lifetime of a VkPhysicalDevice.
     */
    class VulkanPhysicalDevice
    {
    public:
        /**
         * \brief VulkanPhysicalDevice settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan instance.
             */
            ObjectRefSetting<VulkanInstance> instance;

            /**
             * \brief Optional Vulkan surface.
             */
            ObjectRefSetting<VulkanSurface> surface;

            /**
             * \brief List of required device extensions.
             */
            std::vector<std::string> extensions;

            /**
             * \brief Function to filter devices by their properties. Should return true if device fulfills requirements.
             */
            std::function<bool(const VkPhysicalDeviceProperties&)> propertyFilter;

            /**
             * \brief Function to filter devices by their features. Should return true if device fulfills requirements.
             */
            std::function<bool(const VkPhysicalDeviceFeatures&)> featureFilter;

            /**
             * \brief Function to filter devices by their queue families. Should return true if device fulfills requirements.
             */
            std::function<bool(const std::vector<VulkanQueueFamily>&)> queueFamilyFilter;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanPhysicalDevice() = delete;

        VulkanPhysicalDevice(SettingsPtr                                  settingsPtr,
                             VkPhysicalDevice                             physicalDevice,
                             std::vector<VulkanQueueFamily>               families,
                             std::optional<VulkanSwapchainSupportDetails> details);

        VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;

        VulkanPhysicalDevice(VulkanPhysicalDevice&&) = delete;

        ~VulkanPhysicalDevice() noexcept;

        VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = delete;

        VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan physical device.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \throws VulkanNoDevicesError Thrown if no devices were found, or none of the devices fulfilled requirements.
         * \return Vulkan physical device.
         */
        [[nodiscard]] static VulkanPhysicalDevicePtr create(Settings settings);

        /**
         * \brief Create a new Vulkan physical device.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \throws VulkanNoDevicesError Thrown if no devices were found, or none of the devices fulfilled requirements.
         * \return Vulkan physical device.
         */
        [[nodiscard]] static VulkanPhysicalDeviceSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

        /**
         * \brief Get the instance.
         * \return VulkanInstance.
         */
        [[nodiscard]] VulkanInstance& getInstance() noexcept;

        /**
         * \brief Get the instance.
         * \return VulkanInstance.
         */
        [[nodiscard]] const VulkanInstance& getInstance() const noexcept;

        /**
         * \brief Get the device handle managed by this object.
         * \return Device handle.
         */
        [[nodiscard]] const VkPhysicalDevice& get() const noexcept;

        /**
         * \brief Get the list of supported queue families.
         * \return Queue families.
         */
        [[nodiscard]] std::vector<VulkanQueueFamily>& getQueueFamilies() noexcept;

        /**
         * \brief Get the list of supported queue families.
         * \return Queue families.
         */
        [[nodiscard]] const std::vector<VulkanQueueFamily>& getQueueFamilies() const noexcept;

        /**
         * \brief Get the VulkanSwapchainSupportDetails.
         * \return VulkanSwapchainSupportDetails.
         */
        [[nodiscard]] const std::optional<VulkanSwapchainSupportDetails>& getSwapchainSupportDetails() const noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void recreateSwapchainSupportDetails(const VulkanSurface* surface);

    private:
        [[nodiscard]] static std::
          tuple<VkPhysicalDevice, std::vector<VulkanQueueFamily>, std::optional<VulkanSwapchainSupportDetails>>
          createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan physical device.
         */
        VkPhysicalDevice device = VK_NULL_HANDLE;

        /**
         * \brief Queue families.
         */
        std::vector<VulkanQueueFamily> queueFamilies;

        /**
         * \brief Swap chain support details.
         */
        std::optional<VulkanSwapchainSupportDetails> swapchainSupportDetails;
    };
}  // namespace sol