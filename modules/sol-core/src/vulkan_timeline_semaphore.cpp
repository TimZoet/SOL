#include "sol-core/vulkan_timeline_semaphore.h"

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
    VulkanTimelineSemaphore::VulkanTimelineSemaphore(const Settings& set, const VkSemaphore vkSemaphore) :
        settings(set), semaphore(vkSemaphore)
    {
    }
#else
    VulkanTimelineSemaphore::VulkanTimelineSemaphore(const Settings& set, const VkSemaphore vkSemaphore) :
        device(&set.device()), semaphore(vkSemaphore)
    {
    }
#endif

    VulkanTimelineSemaphore::~VulkanTimelineSemaphore() noexcept
    {
        vkDestroySemaphore(getDevice().get(), semaphore, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanTimelineSemaphorePtr VulkanTimelineSemaphore::create(const Settings& settings)
    {
        const auto semaphore = createImpl(settings);
        return std::make_unique<VulkanTimelineSemaphore>(settings, semaphore);
    }

    std::vector<VulkanTimelineSemaphorePtr> VulkanTimelineSemaphore::create(const Settings settings, const size_t count)
    {
        std::vector<VulkanTimelineSemaphorePtr> semaphores;
        semaphores.reserve(count);
        for (size_t i = 0; i < count; i++) semaphores.emplace_back(create(settings));
        return semaphores;
    }

    VulkanTimelineSemaphoreSharedPtr VulkanTimelineSemaphore::createShared(const Settings& settings)
    {
        const auto semaphore = createImpl(settings);
        return std::make_shared<VulkanTimelineSemaphore>(settings, semaphore);
    }

    VkSemaphore VulkanTimelineSemaphore::createImpl(const Settings& settings)
    {
        VkSemaphoreTypeCreateInfo timelineCreateInfo;
        timelineCreateInfo.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        timelineCreateInfo.pNext         = VK_NULL_HANDLE;
        timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        timelineCreateInfo.initialValue  = settings.initialValue;

        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.pNext = &timelineCreateInfo;

        // Create semaphore.
        VkSemaphore semaphore;
        handleVulkanError(vkCreateSemaphore(settings.device, &createInfo, nullptr, &semaphore));

        return semaphore;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanTimelineSemaphore::Settings& VulkanTimelineSemaphore::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanTimelineSemaphore::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanTimelineSemaphore::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkSemaphore& VulkanTimelineSemaphore::get() const noexcept { return semaphore; }
}  // namespace sol