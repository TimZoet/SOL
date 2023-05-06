#include "sol-core-test/utils.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_physical_device.h"

sol::VulkanInstancePtr createDefaultInstance()
{
    sol::VulkanInstance::Settings settings;
    settings.applicationName    = "SolTest";
    settings.applicationVersion = sol::Version(1, 0, 0);
    settings.validationLayers   = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME};
    return sol::VulkanInstance::create(settings);
}

sol::VulkanPhysicalDevicePtr createDefaultPhysicalDevice(sol::VulkanInstance& instance)
{
    sol::VulkanPhysicalDevice::Settings settings;
    settings.instance = instance;
    return sol::VulkanPhysicalDevice::create(settings);
}

sol::VulkanDevicePtr createDefaultDevice(sol::VulkanPhysicalDevice& physicalDevice)
{
    sol::VulkanDevice::Settings settings;
    settings.physicalDevice = physicalDevice;
    settings.extensions     = physicalDevice.getSettings().extensions;
    settings.queues.resize(physicalDevice.getQueueFamilies().size(), 1);
    return sol::VulkanDevice::create(settings);
}
