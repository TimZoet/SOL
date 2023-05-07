#include "sol-core/vulkan_swapchain_support_details.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_surface.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails() = default;

    VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(const VkSurfaceCapabilitiesKHR&  cap,
                                                                 std::vector<VkSurfaceFormatKHR> formats,
                                                                 std::vector<VkPresentModeKHR>   modes) :
        capabilities(cap), surfaceFormats(std::move(formats)), presentModes(std::move(modes))
    {
    }

    VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(const VulkanSwapchainSupportDetails&) = default;

    VulkanSwapchainSupportDetails::VulkanSwapchainSupportDetails(VulkanSwapchainSupportDetails&&) noexcept = default;

    VulkanSwapchainSupportDetails::~VulkanSwapchainSupportDetails() noexcept = default;

    VulkanSwapchainSupportDetails&
      VulkanSwapchainSupportDetails::operator=(const VulkanSwapchainSupportDetails&) = default;

    VulkanSwapchainSupportDetails&
      VulkanSwapchainSupportDetails::operator=(VulkanSwapchainSupportDetails&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSwapchainSupportDetails VulkanSwapchainSupportDetails::create(const VkPhysicalDevice& physicalDevice,
                                                                        const VulkanSurface&    surface)
    {
        VulkanSwapchainSupportDetails details;

        // Retrieve surface capabilities.
        handleVulkanError(
          vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface.get(), &details.capabilities));

        // Get number of supported surface formats.
        uint32_t formatCount;
        handleVulkanError(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface.get(), &formatCount, nullptr));

        // Retrieve supported surface formats.
        if (formatCount != 0)
        {
            details.surfaceFormats.resize(formatCount);
            handleVulkanError(vkGetPhysicalDeviceSurfaceFormatsKHR(
              physicalDevice, surface.get(), &formatCount, details.surfaceFormats.data()));
        }

        // Get number of supported present modes.
        uint32_t presentModeCount;
        handleVulkanError(
          vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface.get(), &presentModeCount, nullptr));

        // Retrieve supported present modes.
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            handleVulkanError(vkGetPhysicalDeviceSurfacePresentModesKHR(
              physicalDevice, surface.get(), &presentModeCount, details.presentModes.data()));
        }

        return details;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VkSurfaceCapabilitiesKHR& VulkanSwapchainSupportDetails::getCapabilities() const noexcept
    {
        return capabilities;
    }

    const std::vector<VkSurfaceFormatKHR>& VulkanSwapchainSupportDetails::getSurfaceFormats() const noexcept
    {
        return surfaceFormats;
    }

    const std::vector<VkPresentModeKHR>& VulkanSwapchainSupportDetails::getPresentModes() const noexcept
    {
        return presentModes;
    }
}  // namespace sol