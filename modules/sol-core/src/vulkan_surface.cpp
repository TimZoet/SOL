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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanSurface::VulkanSurface(const Settings& set, const VkSurfaceKHR vkSurface) : settings(set), surface(vkSurface)
    {
    }
#else
    VulkanSurface::VulkanSurface(const Settings& set, const VkSurfaceKHR vkSurface) :
        instance(&set.instance()), surface(vkSurface)
    {
    }
#endif

    VulkanSurface::~VulkanSurface() noexcept { vkDestroySurfaceKHR(getInstance().get(), surface, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSurfacePtr VulkanSurface::create(const Settings& settings)
    {
        const auto surface = createImpl(settings);
        return std::make_unique<VulkanSurface>(settings, surface);
    }

    VulkanSurfaceSharedPtr VulkanSurface::createShared(const Settings& settings)
    {
        const auto surface = createImpl(settings);
        return std::make_shared<VulkanSurface>(settings, surface);
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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanSurface::Settings& VulkanSurface::getSettings() const noexcept { return settings; }
#endif

    VulkanInstance& VulkanSurface::getInstance() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.instance();
#else
        return *instance;
#endif
    }

    const VulkanInstance& VulkanSurface::getInstance() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.instance();
#else
        return *instance;
#endif
    }

    const VkSurfaceKHR& VulkanSurface::get() const noexcept { return surface; }
}  // namespace sol