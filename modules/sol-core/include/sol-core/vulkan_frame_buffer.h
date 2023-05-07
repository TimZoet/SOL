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
    class VulkanFramebuffer
    {
    public:
        struct Settings
        {
            /**
             * \brief Vulkan render pass.
             */
            ObjectRefSetting<VulkanRenderPass> renderPass;

            /**
             * \brief List of attachments.
             */
            std::vector<VulkanImageView*> attachments;

            /**
             * \brief Width.
             */
            uint32_t width = 0;

            /**
             * \brief Height.
             */
            uint32_t height = 0;

            /**
             * \brief Number of layers.
             */
            uint32_t layers = 1;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanFramebuffer() = delete;

        VulkanFramebuffer(const Settings& set, VkFramebuffer vkFramebuffer);

        VulkanFramebuffer(const VulkanFramebuffer&) = delete;

        VulkanFramebuffer(VulkanFramebuffer&&) = delete;

        ~VulkanFramebuffer() noexcept;

        VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

        VulkanFramebuffer& operator=(VulkanFramebuffer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan framebuffer.
         * \param settings Settings.
         * \throws VulkanError Thrown if framebuffer creation failed.
         * \return Framebuffer.
         */
        [[nodiscard]] static VulkanFramebufferPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan framebuffer.
         * \param settings Settings.
         * \throws VulkanError Thrown if framebuffer creation failed.
         * \return Framebuffer.
         */
        [[nodiscard]] static VulkanFramebufferSharedPtr createShared(const Settings& settings);

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
         * \brief Get the framebuffer handle managed by this object.
         * \return Framebuffer handle.
         */
        [[nodiscard]] const VkFramebuffer& get() const noexcept;

        [[nodiscard]] VkExtent2D getExtent() const noexcept;

    private:
        [[nodiscard]] static VkFramebuffer createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanRenderPass* renderPass = nullptr;

        VkExtent2D extent{};
#endif

        /**
         * \brief Vulkan framebuffer.
         */
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
    };
}  // namespace sol