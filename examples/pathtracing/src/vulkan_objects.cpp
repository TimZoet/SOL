#include "pathtracing/vulkan_objects.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

namespace
{
    std::vector              validationLayers = {std::string("VK_LAYER_KHRONOS_validation")};
    std::vector<std::string> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}  // namespace

sol::WindowPtr createWindow(const int32_t width, const int32_t height)
{
    return std::make_unique<sol::Window>(std::array{width, height}, "ForwardRenderer");
}

sol::VulkanInstancePtr createInstance()
{

    sol::VulkanInstance::Settings instanceSettings;
    instanceSettings.applicationName = "ForwardRenderer";
    instanceSettings.extensions      = sol::Window::getRequiredExtensions();
    instanceSettings.extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instanceSettings.enableDebugging  = true;
    instanceSettings.enableValidation = true;
    instanceSettings.validationLayers = validationLayers;
    return sol::VulkanInstance::create(instanceSettings);
}

sol::VulkanSurfacePtr createSurface(sol::VulkanInstance& instance, sol::Window& window)
{
    sol::VulkanSurface::Settings surfaceSettings;
    surfaceSettings.instance = instance;
    surfaceSettings.func     = [&window](sol::VulkanInstance& instance, VkSurfaceKHR* surface) {
        return glfwCreateWindowSurface(instance.get(), window.get(), nullptr, surface);
    };
    return sol::VulkanSurface::create(surfaceSettings);
}

sol::VulkanPhysicalDevicePtr createPhysicalDevice(sol::VulkanInstance& instance, sol::VulkanSurface& surface)
{
    sol::VulkanPhysicalDevice::Settings physicalDeviceSettings;
    physicalDeviceSettings.instance = instance;
    physicalDeviceSettings.surface  = surface;
    physicalDeviceSettings.extensions.assign(deviceExtensions.begin(), deviceExtensions.end());
    physicalDeviceSettings.propertyFilter = [](const VkPhysicalDeviceProperties& props) {
        return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    };
    physicalDeviceSettings.queueFamilyFilter = [](const std::vector<sol::VulkanQueueFamily>& queues) {
        bool hasGraphics          = false;
        bool hasPresent           = false;
        bool hasDedicatedTransfer = false;

        for (const auto& q : queues)
        {
            if (q.supportsGraphics()) hasGraphics = true;
            if (q.supportsPresent()) hasPresent = true;
            if (q.supportsDedicatedTransfer()) hasDedicatedTransfer = true;
        }

        return hasGraphics && hasPresent && hasDedicatedTransfer;
    };
    return sol::VulkanPhysicalDevice::create(physicalDeviceSettings);
}


sol::VulkanDevicePtr createDevice(sol::VulkanPhysicalDevice& physicalDevice)
{
    sol::VulkanDevice::Settings deviceSettings;
    deviceSettings.physicalDevice = physicalDevice;
    deviceSettings.extensions     = physicalDevice.getSettings().extensions;
    deviceSettings.queues.resize(physicalDevice.getQueueFamilies().size());
    std::ranges::fill(deviceSettings.queues.begin(), deviceSettings.queues.end(), 1);
    deviceSettings.threadSafeQueues = true;
    return sol::VulkanDevice::create(deviceSettings);
}

sol::VulkanSwapchainPtr createSwapchain(sol::VulkanDevice& device, VkExtent2D extent)
{
    auto& physicalDevice = device.getSettings().physicalDevice();
    auto& surface        = physicalDevice.getSettings().surface();

    sol::VulkanSwapchain::Settings swapchainSettings;
    swapchainSettings.surface        = surface;
    swapchainSettings.physicalDevice = physicalDevice;
    swapchainSettings.device         = device;
    swapchainSettings.extent         = extent;
    return sol::VulkanSwapchain::create(swapchainSettings);
}
