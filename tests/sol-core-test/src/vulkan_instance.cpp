#include "sol-core-test/vulkan_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_instance.h"

void VulkanInstance::operator()()
{
    sol::VulkanInstance::Settings settings;

    // Create with all default settings.
    expectThrow([&] { static_cast<void>(sol::VulkanInstance::create(settings)); });
    expectThrow([&] { static_cast<void>(sol::VulkanInstance::createShared(settings)); });

    // Create with some random properties.
    settings.applicationName    = "SolTest";
    settings.applicationVersion = sol::Version(1, 2, 3);
    expectNoThrow([&] { static_cast<void>(sol::VulkanInstance::create(settings)); });
    expectNoThrow([&] {
        const auto instance0 = sol::VulkanInstance::create(settings);
        const auto instance1 = sol::VulkanInstance::create(instance0->getSettings());

        compareNE(VK_NULL_HANDLE, instance0->get());
        compareNE(VK_NULL_HANDLE, instance1->get());
        compareNE(instance0->get(), instance1->get());
    });

    // Enable some non-existent extension.
    settings.extensions.emplace_back("SOME_FAKE_EXTENSION");
    expectThrow([&] { static_cast<void>(sol::VulkanInstance::create(settings)); });

    // TODO: Test valid extensions and validation layers.
}
