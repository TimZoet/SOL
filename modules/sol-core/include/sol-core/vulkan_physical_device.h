#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <any>
#include <expected>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
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
#include "sol-core/vulkan_physical_device_features.h"
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
             * \brief Supported features struct to write to.
             */
            RootVulkanPhysicalDeviceFeatures2* features = nullptr;

            /**
             * \brief Function to filter devices by their features. Should return true if device fulfills requirements.
             */
            std::function<bool(RootVulkanPhysicalDeviceFeatures2&)> featureFilter;

            /**
             * \brief Function to filter devices by their queue families. Should return true if device fulfills requirements.
             */
            std::function<bool(const std::vector<VulkanQueueFamily>&)> queueFamilyFilter;

            [[nodiscard]] std::expected<bool, std::string> validate() const noexcept;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanPhysicalDevice() = delete;

        VulkanPhysicalDevice(const Settings&                              set,
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
        [[nodiscard]] static VulkanPhysicalDevicePtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan physical device.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \throws VulkanNoDevicesError Thrown if no devices were found, or none of the devices fulfilled requirements.
         * \return Vulkan physical device.
         */
        [[nodiscard]] static VulkanPhysicalDeviceSharedPtr createShared(const Settings& settings);

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
         * \brief Get the supported features.
         * \return VulkanPhysicalDeviceFeatures2
         */
        [[nodiscard]] const RootVulkanPhysicalDeviceFeatures2& getSupportedFeatures() const noexcept;

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
        // Properties.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Retrieve physical device properties. Retrieved structs are cached. You can keep a pointer / reference to the struct.
         * \tparam T Properties structure type.
         * \tparam S Matching enum value for T.
         * \return Filled in property struct.
         */
        template<typename T, VkStructureType S>
        [[nodiscard]] const T& getProperties()
        {
            auto it = properties.find(S);

            if (it == properties.end())
            {
                T props{.sType = S};
                props.sType = S;

                VkPhysicalDeviceProperties2 props2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                                                   .pNext = &props};

                vkGetPhysicalDeviceProperties2(device, &props2);
                it = properties.try_emplace(S, std::any(props)).first;
            }

            return std::any_cast<const T&>(it->second);
        }

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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        /**
         * \brief Supported features.
         */
        VulkanPhysicalDeviceFeatures2* features = nullptr;

        VulkanInstance* instance = nullptr;

        VulkanSurface* surface = nullptr;
#endif

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

        std::unordered_map<VkStructureType, std::any> properties;
    };
}  // namespace sol