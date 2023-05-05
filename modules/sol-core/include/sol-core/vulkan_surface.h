#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

namespace sol
{
    /**
     * \brief The VulkanSurface object manages the lifetime of a VkSurfaceKHR.
     */
    class VulkanSurface
    {
    public:
        /**
         * \brief VulkanSurface settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan instance.
             */
            ObjectRefSetting<VulkanInstance> instance;

            std::function<VkResult(VulkanInstance& instance, VkSurfaceKHR* surface)> func;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanSurface() = delete;

        VulkanSurface(SettingsPtr settingsPtr, VkSurfaceKHR vkSurface);

        VulkanSurface(const VulkanSurface&) = delete;

        VulkanSurface(VulkanSurface&&) = delete;

        ~VulkanSurface() noexcept;

        VulkanSurface& operator=(const VulkanSurface&) = delete;

        VulkanSurface& operator=(VulkanSurface&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan surface.
         * \param settings Settings.
         * \throws VulkanError Thrown if surface creation failed.
         * \return Vulkan surface.
         */
        [[nodiscard]] static VulkanSurfacePtr create(Settings settings);

        /**
         * \brief Create a new Vulkan surface.
         * \param settings Settings.
         * \throws VulkanError Thrown if surface creation failed.
         * \return Vulkan surface.
         */
        [[nodiscard]] static VulkanSurfaceSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

        /**
         * \brief Get the surface handle managed by this object.
         * \return Surface handle.
         */
        [[nodiscard]] const VkSurfaceKHR& get() const noexcept;

    private:
        [[nodiscard]] static VkSurfaceKHR createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan surface.
         */
        VkSurfaceKHR surface = VK_NULL_HANDLE;
    };
}  // namespace sol