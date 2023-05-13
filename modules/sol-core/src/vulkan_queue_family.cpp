#include "sol-core/vulkan_queue_family.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_surface.h"

namespace sol
{
    VulkanQueueFamily::VulkanQueueFamily() = default;

    VulkanQueueFamily::VulkanQueueFamily(const VkPhysicalDevice        device,
                                         VulkanSurface*                surf,
                                         const VkQueueFamilyProperties props,
                                         const uint32_t                index) :
        physicalDevice(device), surface(surf), familyProperties(props), familyIndex(index)
    {
        computeSupport  = familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT;
        graphicsSupport = familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        transferSupport = familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT || computeSupport ||
                          graphicsSupport;  // Transfer support is implied by compute or graphics support.

        // Only query for present support when there is a surface.
        if (surface)
        {
            VkBool32 val = false;
            handleVulkanError(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, familyIndex, surface->get(), &val));
            presentSupport = val;
        }
    }

    VulkanQueueFamily::VulkanQueueFamily(const VulkanQueueFamily&) = default;

    VulkanQueueFamily::VulkanQueueFamily(VulkanQueueFamily&&) noexcept = default;

    VulkanQueueFamily ::~VulkanQueueFamily() noexcept = default;

    VulkanQueueFamily& VulkanQueueFamily::operator=(const VulkanQueueFamily&) = default;

    VulkanQueueFamily& VulkanQueueFamily::operator=(VulkanQueueFamily&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VkQueueFamilyProperties& VulkanQueueFamily::get() const noexcept { return familyProperties; }

    uint32_t VulkanQueueFamily::getIndex() const noexcept { return familyIndex; }

    bool VulkanQueueFamily::supportsCompute() const noexcept { return computeSupport; }

    bool VulkanQueueFamily::supportsGraphics() const noexcept { return graphicsSupport; }

    bool VulkanQueueFamily::supportsPresent() const noexcept { return presentSupport; }

    bool VulkanQueueFamily::supportsTransfer() const noexcept { return transferSupport; }

    bool VulkanQueueFamily::supportsDedicatedTransfer() const noexcept
    {
        return supportsTransfer() && !(supportsCompute() || supportsGraphics());
    }
}  // namespace sol
