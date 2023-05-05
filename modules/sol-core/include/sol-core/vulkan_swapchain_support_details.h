#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

namespace sol
{
    class VulkanSwapchainSupportDetails
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanSwapchainSupportDetails();

        VulkanSwapchainSupportDetails(VkSurfaceCapabilitiesKHR        cap,
                                      std::vector<VkSurfaceFormatKHR> formats,
                                      std::vector<VkPresentModeKHR>   modes);

        VulkanSwapchainSupportDetails(const VulkanSwapchainSupportDetails&);

        VulkanSwapchainSupportDetails(VulkanSwapchainSupportDetails&&) noexcept;

        ~VulkanSwapchainSupportDetails() noexcept;

        VulkanSwapchainSupportDetails& operator=(const VulkanSwapchainSupportDetails&);

        VulkanSwapchainSupportDetails& operator=(VulkanSwapchainSupportDetails&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] static VulkanSwapchainSupportDetails create(const VkPhysicalDevice& physicalDevice,
                                                                  const VulkanSurface&    surface);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const VkSurfaceCapabilitiesKHR& getCapabilities() const noexcept;

        [[nodiscard]] const std::vector<VkSurfaceFormatKHR>& getSurfaceFormats() const noexcept;

        [[nodiscard]] const std::vector<VkPresentModeKHR>& getPresentModes() const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VkSurfaceCapabilitiesKHR        capabilities{};
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR>   presentModes;
    };
}  // namespace sol