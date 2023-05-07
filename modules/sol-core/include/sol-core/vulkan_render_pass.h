#pragma once

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
     * \brief The VulkanRenderPass object manages the lifetime of a VkRenderPass.
     */
    class VulkanRenderPass
    {
    public:
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Render pass layout.
             */
            VulkanRenderPassLayout* layout = nullptr;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanRenderPass() = delete;

        VulkanRenderPass(const Settings& set, VkRenderPass vkRenderPass);

        VulkanRenderPass(const VulkanRenderPass&) = delete;

        VulkanRenderPass(VulkanRenderPass&&) = delete;

        ~VulkanRenderPass() noexcept;

        VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

        VulkanRenderPass& operator=(VulkanRenderPass&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan renderpass.
         * \param settings Settings.
         * \throws VulkanError Thrown if renderpass creation failed.
         * \return Renderpass.
         */
        [[nodiscard]] static VulkanRenderPassPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan renderpass.
         * \param settings Settings.
         * \throws VulkanError Thrown if renderpass creation failed.
         * \return Renderpass.
         */
        [[nodiscard]] static VulkanRenderPassSharedPtr createShared(const Settings& settings);

        void destroy();

        void recreate();

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;
#endif

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        /**
         * \brief Get the render pass handle managed by this object.
         * \return Render pass handle.
         */
        [[nodiscard]] const VkRenderPass& get() const noexcept;

    private:
        [[nodiscard]] static VkRenderPass createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;

        /**
         * \brief Vulkan render pass.
         */
        VkRenderPass renderPass = VK_NULL_HANDLE;
    };
}  // namespace sol