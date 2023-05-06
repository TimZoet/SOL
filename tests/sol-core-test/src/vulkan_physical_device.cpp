#include "sol-core-test/vulkan_physical_device.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_physical_device.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core-test/utils.h"

void VulkanPhysicalDevice::operator()()
{
    const auto instance = createDefaultInstance();

    // Create with all default settings.
    sol::VulkanPhysicalDevice::Settings settings;
    expectThrow([&] { static_cast<void>(sol::VulkanPhysicalDevice::create(settings)); });

    // Some valid creates.
    settings.instance = *instance;
    expectNoThrow([&] {
        const auto device = sol::VulkanPhysicalDevice::create(settings);
        compareEQ(&device->getInstance(), instance.get());
        compareGT(device->getQueueFamilies().size(), 0);
    });
    expectNoThrow([&] { static_cast<void>(sol::VulkanPhysicalDevice::createShared(settings)); });
    expectNoThrow([&] {
        const auto device0 = sol::VulkanPhysicalDevice::create(settings);
        const auto device1 = sol::VulkanPhysicalDevice::create(device0->getSettings());

        compareNE(VK_NULL_HANDLE, device0->get());
        compareNE(VK_NULL_HANDLE, device1->get());
        compareEQ(device0->get(), device1->get());
    });

    // Create with various filters.
    settings.propertyFilter = [&](const VkPhysicalDeviceProperties&) { return false; };
    expectThrow([&] { static_cast<void>(sol::VulkanPhysicalDevice::create(settings)); });
    settings.propertyFilter = [&](const VkPhysicalDeviceProperties&) { return true; };
    settings.propertyFilter = {};
    settings.featureFilter  = [&](const VkPhysicalDeviceFeatures&) { return false; };
    expectThrow([&] { static_cast<void>(sol::VulkanPhysicalDevice::create(settings)); });
    settings.featureFilter = [&](const VkPhysicalDeviceFeatures&) { return true; };
    expectNoThrow([&] { static_cast<void>(sol::VulkanPhysicalDevice::create(settings)); });
    settings.featureFilter     = {};
    settings.queueFamilyFilter = [&](const std::vector<sol::VulkanQueueFamily>&) { return false; };
    expectThrow([&] { static_cast<void>(sol::VulkanPhysicalDevice::create(settings)); });
    settings.queueFamilyFilter = [&](const std::vector<sol::VulkanQueueFamily>&) { return true; };
    expectNoThrow([&] { static_cast<void>(sol::VulkanPhysicalDevice::create(settings)); });
    settings.queueFamilyFilter = {};

    // TODO: Test extensions.
    // TODO: Test surface and swapchain.
}
