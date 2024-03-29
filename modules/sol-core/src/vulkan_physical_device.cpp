#include "sol-core/vulkan_physical_device.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/settings_validation_error.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-error/vulkan_no_devices_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_surface.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanPhysicalDevice::VulkanPhysicalDevice(const Settings&                              set,
                                               const VkPhysicalDevice                       physicalDevice,
                                               std::vector<VulkanQueueFamily>               families,
                                               std::optional<VulkanSwapchainSupportDetails> details) :
        settings(set),
        device(physicalDevice),
        queueFamilies(std::move(families)),
        swapchainSupportDetails(std::move(details))
    {
    }
#else
    VulkanPhysicalDevice::VulkanPhysicalDevice(const Settings&                              set,
                                               const VkPhysicalDevice                       physicalDevice,
                                               std::vector<VulkanQueueFamily>               families,
                                               std::optional<VulkanSwapchainSupportDetails> details) :
        features(set.features),
        instance(&set.instance()),
        surface(set.surface ? &set.surface() : nullptr),
        device(physicalDevice),
        queueFamilies(std::move(families)),
        swapchainSupportDetails(std::move(details))
    {
    }
#endif

    VulkanPhysicalDevice::~VulkanPhysicalDevice() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanPhysicalDevicePtr VulkanPhysicalDevice::create(const Settings& settings)
    {
        const auto [device, indices, details] = createImpl(settings);
        return std::make_unique<VulkanPhysicalDevice>(settings, device, indices, details);
    }

    VulkanPhysicalDeviceSharedPtr VulkanPhysicalDevice::createShared(const Settings& settings)
    {
        const auto [device, indices, details] = createImpl(settings);
        return std::make_shared<VulkanPhysicalDevice>(settings, device, indices, details);
    }

    std::expected<bool, std::string> VulkanPhysicalDevice::Settings::validate() const noexcept
    {
        if (!instance) return std::unexpected("No instance set.");
        if (!std::ranges::contains(extensions, VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME))
            return std::unexpected(std::format("The required device extension {} was not enabled.",
                                               VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME));
        return true;
    }

    std::tuple<VkPhysicalDevice, std::vector<VulkanQueueFamily>, std::optional<VulkanSwapchainSupportDetails>>
      VulkanPhysicalDevice::createImpl(const Settings& settings)
    {
        if (const auto exp = settings.validate(); !exp.has_value())
            throw SettingsValidationError(
              std::format("Could not create VulkanPhysicalDevice. Settings not valid: {}", exp.error()));

        // Get number of devices.
        uint32_t deviceCount = 0;
        handleVulkanError(vkEnumeratePhysicalDevices(settings.instance, &deviceCount, nullptr));
        if (deviceCount == 0) { throw VulkanNoDevicesError("vkEnumeratePhysicalDevices did not return any devices."); }

        // Retrieve devices.
        std::vector<VkPhysicalDevice> devices(deviceCount);
        handleVulkanError(vkEnumeratePhysicalDevices(settings.instance, &deviceCount, devices.data()));

        // Select best device.
        for (const auto& d : devices)
        {
            // Filter device based on its properties.
            if (settings.propertyFilter)
            {
                // Get device properties.
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(d, &deviceProperties);

                // Run filter. If it fails, skip current device.
                if (!settings.propertyFilter(deviceProperties)) continue;
            }

            // Filter device based on its features.
            vkGetPhysicalDeviceFeatures2(d, &settings.features->features);
            if (settings.featureFilter && !settings.featureFilter(*settings.features)) continue;

            // Retrieve queue families.
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> vkQueueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyCount, vkQueueFamilies.data());
            std::vector<VulkanQueueFamily> queueFamilies;
            for (size_t i = 0; i < vkQueueFamilies.size(); i++)
                queueFamilies.emplace_back(
                  d, settings.surface ? &settings.surface() : nullptr, vkQueueFamilies[i], static_cast<uint32_t>(i));

            // Filter device based on its queue families.
            if (settings.queueFamilyFilter)
            {
                if (!settings.queueFamilyFilter(queueFamilies)) continue;
            }

            // Check if all required extensions are available.
            if (!settings.extensions.empty())
            {
                // Get available extensions.
                uint32_t extensionCount;
                handleVulkanError(vkEnumerateDeviceExtensionProperties(d, nullptr, &extensionCount, nullptr));
                std::vector<VkExtensionProperties> availableExtensions(extensionCount);
                handleVulkanError(
                  vkEnumerateDeviceExtensionProperties(d, nullptr, &extensionCount, availableExtensions.data()));

                // Check if all required extensions are available.
                bool success = true;
                for (const auto& ext : settings.extensions)
                {
                    if (std::ranges::find_if(availableExtensions.begin(),
                                             availableExtensions.end(),
                                             [&ext](const VkExtensionProperties& prop) {
                                                 return ext == prop.extensionName;
                                             }) == availableExtensions.end())
                    {
                        success = false;
                        break;
                    }
                }

                // Missing extension(s), skip current device.
                if (!success) continue;
            }

            // If there is a surface get swapchain.
            std::optional<VulkanSwapchainSupportDetails> swapchainDetails;
            if (settings.surface)
            {
                swapchainDetails = VulkanSwapchainSupportDetails::create(d, settings.surface());
                if ((*swapchainDetails).getSurfaceFormats().empty() || (*swapchainDetails).getPresentModes().empty())
                    continue;
            }

            return {d, std::move(queueFamilies), std::move(swapchainDetails)};
        }

        throw VulkanNoDevicesError("Did not find any physical device that fulfilled requirements.");
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const RootVulkanPhysicalDeviceFeatures2& VulkanPhysicalDevice::getSupportedFeatures() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return *settings.features;
#else
        return *features;
#endif
    }

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanPhysicalDevice::Settings& VulkanPhysicalDevice::getSettings() const noexcept { return settings; }
#endif

    VulkanInstance& VulkanPhysicalDevice::getInstance() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.instance();
#else
        return *instance;
#endif
    }

    const VulkanInstance& VulkanPhysicalDevice::getInstance() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.instance();
#else
        return *instance;
#endif
    }

    const VkPhysicalDevice& VulkanPhysicalDevice::get() const noexcept { return device; }

    std::vector<VulkanQueueFamily>& VulkanPhysicalDevice::getQueueFamilies() noexcept { return queueFamilies; }

    const std::vector<VulkanQueueFamily>& VulkanPhysicalDevice::getQueueFamilies() const noexcept
    {
        return queueFamilies;
    }

    const std::optional<VulkanSwapchainSupportDetails>&
      VulkanPhysicalDevice::getSwapchainSupportDetails() const noexcept
    {
        return swapchainSupportDetails;
    }

    void VulkanPhysicalDevice::recreateSwapchainSupportDetails(const VulkanSurface* surface)
    {
        // TODO: This surface parameter was added because these details must be recreated when dealing with multiple surfaces.
        // The surface in the settings of this physdevice then is not always the correct one. That means that the details and
        // surface could go out of sync without that being very clear. How to deal with that? Just get rid of the surface and
        // details members and let user keep track of that stuff?
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        swapchainSupportDetails =
          VulkanSwapchainSupportDetails::create(device, surface ? *surface : settings.surface());
#else
        swapchainSupportDetails = VulkanSwapchainSupportDetails::create(device, surface ? *surface : *this->surface);
#endif
    }

}  // namespace sol