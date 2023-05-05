#include "sol-core/vulkan_semaphore.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanSemaphore::VulkanSemaphore(SettingsPtr settingsPtr, const VkSemaphore vkSemaphore) :
        settings(std::move(settingsPtr)), semaphore(vkSemaphore)
    {
    }

    VulkanSemaphore::~VulkanSemaphore() noexcept { vkDestroySemaphore(settings->device, semaphore, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSemaphorePtr VulkanSemaphore::create(Settings settings)
    {
        const auto semaphore = createImpl(settings);
        return std::make_unique<VulkanSemaphore>(std::make_unique<Settings>(settings), semaphore);
    }

    std::vector<VulkanSemaphorePtr> VulkanSemaphore::create(const Settings settings, const size_t count)
    {
        std::vector<VulkanSemaphorePtr> semaphores;
        semaphores.reserve(count);
        for (size_t i = 0; i < count; i++) semaphores.emplace_back(create(settings));
        return semaphores;
    }

    VulkanSemaphoreSharedPtr VulkanSemaphore::createShared(Settings settings)
    {
        const auto semaphore = createImpl(settings);
        return std::make_shared<VulkanSemaphore>(std::make_unique<Settings>(settings), semaphore);
    }

    VkSemaphore VulkanSemaphore::createImpl(const Settings& settings)
    {
        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Create semaphore.
        VkSemaphore semaphore;
        handleVulkanError(vkCreateSemaphore(settings.device, &createInfo, nullptr, &semaphore));

        return semaphore;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanSemaphore::Settings& VulkanSemaphore::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanSemaphore::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanSemaphore::getDevice() const noexcept { return settings->device(); }

    const VkSemaphore& VulkanSemaphore::get() const noexcept { return semaphore; }
}  // namespace sol