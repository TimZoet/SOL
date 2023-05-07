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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanSemaphore::VulkanSemaphore(const Settings& set, const VkSemaphore vkSemaphore) :
        settings(set), semaphore(vkSemaphore)
    {
    }
#else
    VulkanSemaphore::VulkanSemaphore(const Settings& set, const VkSemaphore vkSemaphore) :
        device(&set.device()), semaphore(vkSemaphore)
    {
    }
#endif

    VulkanSemaphore::~VulkanSemaphore() noexcept { vkDestroySemaphore(getDevice().get(), semaphore, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSemaphorePtr VulkanSemaphore::create(const Settings& settings)
    {
        const auto semaphore = createImpl(settings);
        return std::make_unique<VulkanSemaphore>(settings, semaphore);
    }

    std::vector<VulkanSemaphorePtr> VulkanSemaphore::create(const Settings settings, const size_t count)
    {
        std::vector<VulkanSemaphorePtr> semaphores;
        semaphores.reserve(count);
        for (size_t i = 0; i < count; i++) semaphores.emplace_back(create(settings));
        return semaphores;
    }

    VulkanSemaphoreSharedPtr VulkanSemaphore::createShared(const Settings& settings)
    {
        const auto semaphore = createImpl(settings);
        return std::make_shared<VulkanSemaphore>(settings, semaphore);
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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanSemaphore::Settings& VulkanSemaphore::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanSemaphore::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanSemaphore::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkSemaphore& VulkanSemaphore::get() const noexcept { return semaphore; }
}  // namespace sol