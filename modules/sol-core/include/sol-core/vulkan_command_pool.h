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

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanCommandPool() = delete;

        VulkanCommandPool(const Settings& set, VkCommandPool vkCommandPool);

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
        [[nodiscard]] static VulkanCommandPoolPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan command pool.
         * \param settings Settings.
         * \throws VulkanError Thrown if command pool creation failed.
         * \return Vulkan command pool.
         */
        [[nodiscard]] static VulkanCommandPoolSharedPtr createShared(const Settings& settings);

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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanDevice* device = nullptr;

        VkCommandPoolCreateFlags flags = 0;
#endif

        /**
         * \brief Vulkan command pool.
         */
        VkCommandPool commandPool = VK_NULL_HANDLE;
    };
}  // namespace sol