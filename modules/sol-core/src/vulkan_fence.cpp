#include "sol-core/vulkan_fence.h"

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
    VulkanFence::VulkanFence(const Settings& set, const VkFence vkFence) : settings(set), fence(vkFence) {}
#else
    VulkanFence::VulkanFence(const Settings& set, const VkFence vkFence) : device(&set.device()), fence(vkFence) {}
#endif

    VulkanFence::~VulkanFence() noexcept { vkDestroyFence(getDevice().get(), fence, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanFencePtr VulkanFence::create(const Settings& settings)
    {
        const auto fence = createImpl(settings);
        return std::make_unique<VulkanFence>(settings, fence);
    }

    std::vector<VulkanFencePtr> VulkanFence::create(const Settings settings, const size_t count)
    {
        std::vector<VulkanFencePtr> fences;
        fences.reserve(count);
        for (size_t i = 0; i < count; i++) fences.emplace_back(create(settings));
        return fences;
    }

    VulkanFenceSharedPtr VulkanFence::createShared(const Settings& settings)
    {
        const auto fence = createImpl(settings);
        return std::make_shared<VulkanFence>(settings, fence);
    }

    VkFence VulkanFence::createImpl(const Settings& settings)
    {
        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = settings.signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        // Create fence.
        VkFence fence;
        handleVulkanError(vkCreateFence(settings.device, &createInfo, nullptr, &fence));

        return fence;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanFence::Settings& VulkanFence::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanFence::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanFence::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkFence& VulkanFence::get() const noexcept { return fence; }

    bool VulkanFence::isSignaled() const noexcept { return signaled; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void VulkanFence::setSignaled(const bool value) noexcept { signaled = value; }

}  // namespace sol