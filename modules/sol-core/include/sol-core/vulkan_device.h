#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>
#include <vector>

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
     * \brief The VulkanDevice object manages the lifetime of a VkDevice.
     */
    class VulkanDevice
    {
    public:
        /**
         * \brief VulkanDevice settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan physical device.
             */
            ObjectRefSetting<VulkanPhysicalDevice> physicalDevice;

            /**
             * \brief List of required device extensions.
             */
            std::vector<std::string> extensions;

            /**
             * \brief Number of queues to create for each queue family.
             */
            std::vector<uint32_t> queues;

            /**
             * \brief If true, makes all queues thread safe on construction.
             */
            bool threadSafeQueues = false;

            [[nodiscard]] bool validate() const noexcept;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanDevice() = delete;

        VulkanDevice(SettingsPtr settingsPtr, VkDevice vkDevice);

        VulkanDevice(const VulkanDevice&) = delete;

        VulkanDevice(VulkanDevice&&) = delete;

        ~VulkanDevice() noexcept;

        VulkanDevice& operator=(const VulkanDevice&) = delete;

        VulkanDevice& operator=(VulkanDevice&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan device.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \return Vulkan device.
         */
        [[nodiscard]] static VulkanDevicePtr create(Settings settings);

        /**
         * \brief Create a new Vulkan device.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \return Vulkan device.
         */
        [[nodiscard]] static VulkanDeviceSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

        /**
         * \brief Get the physical device.
         * \return VulkanPhysicalDevice.
         */
        [[nodiscard]] VulkanPhysicalDevice& getPhysicalDevice() noexcept;

        /**
         * \brief Get the physical device.
         * \return VulkanPhysicalDevice.
         */
        [[nodiscard]] const VulkanPhysicalDevice& getPhysicalDevice() const noexcept;

        /**
         * \brief Get the device handle managed by this object.
         * \return Device handle.
         */
        [[nodiscard]] const VkDevice& get() const noexcept;

        /**
         * \brief Get the list of all queues created for this device.
         * \return List of queues.
         */
        [[nodiscard]] const std::vector<VulkanQueuePtr>& getQueues() const noexcept;

        /**
         * \brief Get the list of queues of a specific family created for this device.
         * \param family Queue family.
         * \return List of queues.
         */
        [[nodiscard]] std::vector<VulkanQueue*> getQueues(const VulkanQueueFamily& family) const;

    private:
        [[nodiscard]] static VkDevice createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan device.
         */
        VkDevice device = VK_NULL_HANDLE;

        /**
         * \brief List of queues.
         */
        std::vector<VulkanQueuePtr> queues;
    };
}  // namespace sol