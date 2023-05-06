#include "sol-core-test/vulkan_device.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_physical_device.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core-test/utils.h"

void VulkanDevice::operator()()
{
    const auto instance       = createDefaultInstance();
    const auto physicalDevice = createDefaultPhysicalDevice(*instance);

    // Create with all default settings.
    sol::VulkanDevice::Settings settings;
    expectThrow([&] {
        static_cast<void>(sol::VulkanDevice::create(settings));
        static_cast<void>(sol::VulkanDevice::createShared(settings));
    });

    // Create and recreate with (almost) all default settings.
    settings.physicalDevice = *physicalDevice;
    expectNoThrow([&] {
        const auto device = sol::VulkanDevice::create(settings);
        compareEQ(&device->getPhysicalDevice(), physicalDevice.get());
        compareEQ(device->getQueues().size(), 0);
    });
    expectNoThrow([&] { static_cast<void>(sol::VulkanDevice::createShared(settings)); });
    expectNoThrow([&] {
        const auto device0 = sol::VulkanDevice::create(settings);
        const auto device1 = sol::VulkanDevice::create(device0->getSettings());

        compareNE(VK_NULL_HANDLE, device0->get());
        compareNE(VK_NULL_HANDLE, device1->get());
        compareNE(device0->get(), device1->get());
    });

    // Create with a specific number of queues.
    size_t expectedQueues = 0;
    for (size_t i = 0; i < physicalDevice->getQueueFamilies().size(); i++)
    {
        settings.queues.emplace_back(1);
        expectedQueues++;
    }
    expectNoThrow([&] {
        const auto device = sol::VulkanDevice::create(settings);

        compareEQ(device->getQueues().size(), expectedQueues);

        for (const auto& family : physicalDevice->getQueueFamilies()) compareEQ(device->getQueues(family).size(), 1);
    });

    // TODO: Test extensions and thread safe queues.
}
