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
    class VulkanDeviceMemory
    {
    public:
        /**
         * \brief VulkanDeviceMemory settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Size in bytes.
             */
            size_t size = 0;

            /**
             * \brief M.
             */
            uint32_t memoryTypeBits = 0;

            VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanDeviceMemory() = delete;

        VulkanDeviceMemory(SettingsPtr settingsPtr, VkDeviceMemory vkMemory);

        VulkanDeviceMemory(const VulkanDeviceMemory&) = delete;

        VulkanDeviceMemory(VulkanDeviceMemory&&) = delete;

        ~VulkanDeviceMemory() noexcept;

        VulkanDeviceMemory& operator=(const VulkanDeviceMemory&) = delete;

        VulkanDeviceMemory& operator=(VulkanDeviceMemory&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan device memory.
         * \param settings Settings.
         * \throws VulkanError Thrown if device memory creation failed.
         * \return Vulkan device memory.
         */
        [[nodiscard]] static VulkanDeviceMemoryPtr create(Settings settings);

        /**
         * \brief Create a new Vulkan device memory.
         * \param settings Settings.
         * \throws VulkanError Thrown if device memory creation failed.
         * \return Vulkan device memory.
         */
        [[nodiscard]] static VulkanDeviceMemorySharedPtr createShared(Settings settings);

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
         * \brief Get the device memory handle managed by this object.
         * \return Device memory handle.
         */
        [[nodiscard]] const VkDeviceMemory& get() const noexcept;

    private:
        [[nodiscard]] static VkDeviceMemory createImpl(const Settings& settings);

        [[nodiscard]] static uint32_t
          findMemoryType(const VulkanPhysicalDevice& physicalDevice, uint32_t filter, VkMemoryPropertyFlags properties);

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan device memory.
         */
        VkDeviceMemory memory = VK_NULL_HANDLE;
    };
}  // namespace sol