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

    VulkanFence::VulkanFence(SettingsPtr settingsPtr, const VkFence vkFence) :
        settings(std::move(settingsPtr)), fence(vkFence)
    {
    }

    VulkanFence::~VulkanFence() noexcept { vkDestroyFence(settings->device, fence, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanFencePtr VulkanFence::create(Settings settings)
    {
        const auto fence = createImpl(settings);
        return std::make_unique<VulkanFence>(std::make_unique<Settings>(settings), fence);
    }

    std::vector<VulkanFencePtr> VulkanFence::create(const Settings settings, const size_t count)
    {
        std::vector<VulkanFencePtr> fences;
        fences.reserve(count);
        for (size_t i = 0; i < count; i++) fences.emplace_back(create(settings));
        return fences;
    }

    VulkanFenceSharedPtr VulkanFence::createShared(Settings settings)
    {
        const auto fence = createImpl(settings);
        return std::make_shared<VulkanFence>(std::make_unique<Settings>(settings), fence);
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

    const VulkanFence::Settings& VulkanFence::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanFence::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanFence::getDevice() const noexcept { return settings->device(); }

    const VkFence& VulkanFence::get() const noexcept { return fence; }

    bool VulkanFence::isSignaled() const noexcept { return signaled; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void VulkanFence::setSignaled(const bool value) noexcept { signaled = value; }

}  // namespace sol