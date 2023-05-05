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
    class VulkanCommandPool
    {
    public:
        /**
         * \brief VulkanCommandPool settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Queue family index.
             */
            uint32_t queueFamilyIndex = static_cast<uint32_t>(-1);

            /**
             * \brief Flags.
             */
            VkCommandPoolCreateFlags flags = 0;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanCommandPool() = delete;

        VulkanCommandPool(SettingsPtr settingsPtr, VkCommandPool vkCommandPool);

        VulkanCommandPool(const VulkanCommandPool&) = delete;

        VulkanCommandPool(VulkanCommandPool&&) = delete;

        ~VulkanCommandPool() noexcept;

        VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;

        VulkanCommandPool& operator=(VulkanCommandPool&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan command pool.
         * \param settings Settings.
         * \throws VulkanError Thrown if command pool creation failed.
         * \return Vulkan command pool.
         */
        [[nodiscard]] static VulkanCommandPoolPtr create(Settings settings);

        /**
         * \brief Create a new Vulkan command pool.
         * \param settings Settings.
         * \throws VulkanError Thrown if command pool creation failed.
         * \return Vulkan command pool.
         */
        [[nodiscard]] static VulkanCommandPoolSharedPtr createShared(Settings settings);

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
         * \brief Get the command pool handle managed by this object.
         * \return Command pool handle.
         */
        [[nodiscard]] const VkCommandPool& get() const noexcept;

        /**
         * \brief Returns whether command buffers allocated from this command pool can be reset.
         * \return True if resettable, false otherwise.
         */
        [[nodiscard]] bool isResettable() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Command buffers.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanCommandBufferPtr createCommandBuffer(VkCommandBufferLevel level);

    private:
        [[nodiscard]] static VkCommandPool createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan command pool.
         */
        VkCommandPool commandPool = VK_NULL_HANDLE;
    };
}  // namespace sol