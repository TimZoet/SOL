#include "sol-core/vulkan_instance.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/settings_validation_error.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-error/vulkan_missing_validation_layer_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_debug.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanInstance::VulkanInstance(SettingsPtr                    settingsPtr,
                                   const VkInstance               vkInstance,
                                   const VkDebugUtilsMessengerEXT vkDebugMessenger) :
        settings(std::move(settingsPtr)), instance(vkInstance), debugMessenger(vkDebugMessenger)
    {
    }

    VulkanInstance::~VulkanInstance() noexcept
    {
        if (debugMessenger) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanInstancePtr VulkanInstance::create(Settings settings)
    {
        const auto [instance, messenger] = createImpl(settings);
        return std::make_unique<VulkanInstance>(std::make_unique<Settings>(std::move(settings)), instance, messenger);
    }

    VulkanInstanceSharedPtr VulkanInstance::createShared(Settings settings)
    {
        const auto [instance, messenger] = createImpl(settings);
        return std::make_shared<VulkanInstance>(std::make_unique<Settings>(std::move(settings)), instance, messenger);
    }

    bool VulkanInstance::Settings::validate() const noexcept
    {
        if (applicationName.empty()) return false;

        return true;
    }

    std::pair<VkInstance, VkDebugUtilsMessengerEXT> VulkanInstance::createImpl(const Settings& settings)
    {
        if (!settings.validate()) throw SettingsValidationError("Could not create VulkanInstance. Settings not valid.");

        // Prepare application info.
        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion         = Version::getApiVersion();
        appInfo.pApplicationName   = settings.applicationName.c_str();
        appInfo.applicationVersion = settings.applicationVersion.get();
        appInfo.pEngineName        = "SOL";
        appInfo.engineVersion      = Version::getEngineVersion();

        // Prepare creation info.
        VkInstanceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo        = &appInfo;
        const auto cstrings                = stringVectorToConstCharVector(settings.extensions);
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(cstrings.size());
        createInfo.ppEnabledExtensionNames = cstrings.data();
        createInfo.enabledLayerCount       = 0;
        createInfo.pNext                   = nullptr;

        // Set validation layers.
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        const auto                         layers = stringVectorToConstCharVector(settings.validationLayers);
        if (settings.enableValidation)
        {
            // Get available layers.
            uint32_t layerCount;
            handleVulkanError(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
            std::vector<VkLayerProperties> availableLayers(layerCount);
            handleVulkanError(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

            // Check if all layers are available.
            for (const auto& layer : settings.validationLayers)
            {
                if (std::ranges::find_if(
                      availableLayers.begin(), availableLayers.end(), [&layer](const VkLayerProperties& prop) {
                          return layer == prop.layerName;
                      }) == availableLayers.end())
                {
                    throw VulkanMissingValidationLayerError(std::format("Missing validation layer {}.", layer));
                }
            }

            // Set layers.
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.enabledLayerCount   = static_cast<uint32_t>(layers.size());
            createInfo.ppEnabledLayerNames = layers.data();
            createInfo.pNext               = &debugCreateInfo;
        }

        // Create instance.
        VkInstance instance;
        handleVulkanError(vkCreateInstance(&createInfo, nullptr, &instance));

        // Create debug messenger.
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        if (settings.enableDebugging)
        {
            if (std::ranges::find(settings.extensions.begin(),
                                  settings.extensions.end(),
                                  VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == settings.extensions.end())
                throw SolError("When enableDebugging is true, the debug extension must be enabled explicitly.");

            setupDebugMessenger(instance, debugMessenger);
        }

        return {instance, debugMessenger};
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanInstance::Settings& VulkanInstance::getSettings() const noexcept { return *settings; }

    const VkInstance& VulkanInstance::get() const noexcept { return instance; }
}  // namespace sol