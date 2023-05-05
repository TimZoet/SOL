#pragma once

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
    class VulkanQueueFamily
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanQueueFamily();

        VulkanQueueFamily(VkPhysicalDevice device, VulkanSurface* surf, VkQueueFamilyProperties props, uint32_t index);

        VulkanQueueFamily(const VulkanQueueFamily&);

        VulkanQueueFamily(VulkanQueueFamily&&) noexcept;

        ~VulkanQueueFamily() noexcept;

        VulkanQueueFamily& operator=(const VulkanQueueFamily&);

        VulkanQueueFamily& operator=(VulkanQueueFamily&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const VkQueueFamilyProperties& get() const noexcept;

        [[nodiscard]] uint32_t getIndex() const noexcept;

        [[nodiscard]] bool supportsCompute() const noexcept;

        [[nodiscard]] bool supportsGraphics() const noexcept;

        [[nodiscard]] bool supportsPresent() const noexcept;

        [[nodiscard]] bool supportsTransfer() const noexcept;

        [[nodiscard]] bool supportsDedicatedTransfer() const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VulkanSurface* surface = nullptr;

        VkQueueFamilyProperties familyProperties{};

        uint32_t familyIndex = 0;

        bool computeSupport = false;

        bool graphicsSupport = false;

        bool presentSupport = false;

        bool transferSupport = false;
    };
}  // namespace sol