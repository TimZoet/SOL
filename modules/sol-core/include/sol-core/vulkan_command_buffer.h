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
    class VulkanCommandBuffer
    {
    public:
        /**
         * \brief VulkanCommandBuffer settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan command pool.
             */
            ObjectRefSetting<VulkanCommandPool> commandPool;

            VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanCommandBuffer() = delete;

        VulkanCommandBuffer(SettingsPtr settingsPtr, VkCommandBuffer vkCommandBuffer);

        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer(VulkanCommandBuffer&&) = delete;

        ~VulkanCommandBuffer() noexcept;

        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan command buffer.
         * \param settings Settings.
         * \throws VulkanError Thrown if command buffer creation failed.
         * \return Vulkan command buffer.
         */
        [[nodiscard]] static VulkanCommandBufferPtr create(Settings settings);

        /**
         * \brief Create a new Vulkan command buffer.
         * \param settings Settings.
         * \throws VulkanError Thrown if command buffer creation failed.
         * \return Vulkan command buffer.
         */
        [[nodiscard]] static VulkanCommandBufferSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

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
         * \brief Get the command buffer handle managed by this object.
         * \return Command buffer handle.
         */
        [[nodiscard]] const VkCommandBuffer& get() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Commands.
        ////////////////////////////////////////////////////////////////

        void beginCommand() const;

        void beginOneTimeCommand() const;

        void endCommand() const;

        void resetCommand(VkCommandBufferResetFlags flags) const;

    private:
        [[nodiscard]] static VkCommandBuffer createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan command buffer.
         */
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    };
}  // namespace sol