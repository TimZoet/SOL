#include "sol-core/vulkan_surface.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_instance.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanSurface::VulkanSurface(SettingsPtr settingsPtr, const VkSurfaceKHR vkSurface) :
        settings(std::move(settingsPtr)), surface(vkSurface)
    {
    }

    VulkanSurface::~VulkanSurface() noexcept { vkDestroySurfaceKHR(settings->instance, surface, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSurfacePtr VulkanSurface::create(Settings settings)
    {
        const auto surface = createImpl(settings);
        return std::make_unique<VulkanSurface>(std::make_unique<Settings>(std::move(settings)), surface);
    }

    VulkanSurfaceSharedPtr VulkanSurface::createShared(Settings settings)
    {
        const auto surface = createImpl(settings);
        return std::make_shared<VulkanSurface>(std::make_unique<Settings>(std::move(settings)), surface);
    }

    VkSurfaceKHR VulkanSurface::createImpl(const Settings& settings)
    {
        // Create surface.
        VkSurfaceKHR surface;
        handleVulkanError(settings.func(settings.instance(), &surface));

        return surface;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanSurface::Settings& VulkanSurface::getSettings() const noexcept { return *settings; }

    const VkSurfaceKHR& VulkanSurface::get() const noexcept { return surface; }
}  // namespace sol