#include "testutils/utils.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_physical_device.h"

namespace
{
    sol::VulkanInstancePtr                instance;
    sol::VulkanPhysicalDeviceFeatures2Ptr supportedFeatures;
    sol::VulkanPhysicalDeviceFeatures2Ptr enabledFeatures;
    sol::VulkanPhysicalDevicePtr          physicalDevice;
    sol::VulkanDevicePtr                  device;
}  // namespace

void createDefaultInstance()
{
    sol::VulkanInstance::Settings settings;
    settings.applicationName    = "SolTest";
    settings.applicationVersion = sol::Version(1, 0, 0);
    instance                    = sol::VulkanInstance::create(settings);
}

void createSupportedFeatures()
{
    supportedFeatures =
      std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                          sol::VulkanPhysicalDeviceVulkan12Features,
                                                          sol::VulkanPhysicalDeviceVulkan13Features>>();
}

void createEnabledFeatures()
{
    enabledFeatures = std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                                          sol::VulkanPhysicalDeviceVulkan12Features,
                                                                          sol::VulkanPhysicalDeviceVulkan13Features>>();

    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing  = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering    = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2    = VK_TRUE;
}

void createDefaultPhysicalDevice()
{
    sol::VulkanPhysicalDevice::Settings settings;
    settings.instance       = instance;
    settings.propertyFilter = [](const VkPhysicalDeviceProperties& props) {
        return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    };
    // TODO: Many of these features are kinda needed by SOL out of the box. Perhaps something needs to be built in to SOL to improve this.
    settings.features      = supportedFeatures.get();
    settings.featureFilter = [](sol::RootVulkanPhysicalDeviceFeatures2& features) {
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2) return false;
        return true;
    };
    settings.queueFamilyFilter = [](const std::vector<sol::VulkanQueueFamily>& queues) {
        bool hasGraphics          = false;
        bool hasDedicatedTransfer = false;

        for (const auto& q : queues)
        {
            if (q.supportsGraphics()) hasGraphics = true;
            if (q.supportsDedicatedTransfer()) hasDedicatedTransfer = true;
        }

        return hasGraphics && hasDedicatedTransfer;
    };
    physicalDevice = sol::VulkanPhysicalDevice::create(settings);
}

void createDefaultDevice()
{
    sol::VulkanDevice::Settings settings;
    settings.physicalDevice = physicalDevice;
    settings.extensions     = physicalDevice->getSettings().extensions;
    settings.features       = enabledFeatures.get();
    settings.queues.resize(physicalDevice->getQueueFamilies().size(), 1);
    settings.threadSafeQueues = true;
    device                    = sol::VulkanDevice::create(settings);
}

BasicFixture::BasicFixture()
{
    createDefaultInstance();
    createSupportedFeatures();
    createEnabledFeatures();
    createDefaultPhysicalDevice();
    createDefaultDevice();
}

BasicFixture::~BasicFixture() noexcept
{
    device.reset();
    physicalDevice.reset();
    enabledFeatures.reset();
    supportedFeatures.reset();
    instance.reset();
}

sol::VulkanInstance& BasicFixture::getInstance() { return *instance; }

sol::VulkanPhysicalDevice& BasicFixture::getPhysicalDevice() { return *physicalDevice; }

sol::VulkanDevice& BasicFixture::getDevice() { return *device; }
