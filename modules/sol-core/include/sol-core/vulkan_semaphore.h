#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

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
    class VulkanSemaphore
    {
    public:
        /**
         * \brief VulkanSemaphore settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanSemaphore() = delete;

        VulkanSemaphore(SettingsPtr settingsPtr, VkSemaphore vkSemaphore);

        VulkanSemaphore(const VulkanSemaphore&) = delete;

        VulkanSemaphore(VulkanSemaphore&&) = delete;

        ~VulkanSemaphore() noexcept;

        VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;

        VulkanSemaphore& operator=(VulkanSemaphore&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan semaphore.
         * \param settings Settings.
         * \throws VulkanError Thrown if semaphore creation failed.
         * \return Vulkan semaphore.
         */
        [[nodiscard]] static VulkanSemaphorePtr create(Settings settings);

        /**
         * \brief Create a list of new Vulkan semaphores.
         * \param settings Settings.
         * \param count Number of semaphores to create.
         * \throws VulkanError Thrown if semaphore creation failed.
         * \return List of Vulkan semaphores.
         */
        [[nodiscard]] static std::vector<VulkanSemaphorePtr> create(Settings settings, size_t count);

        /**
         * \brief Create a new Vulkan semaphore.
         * \param settings Settings.
         * \throws VulkanError Thrown if semaphore creation failed.
         * \return Vulkan semaphore.
         */
        [[nodiscard]] static VulkanSemaphoreSharedPtr createShared(Settings settings);

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
         * \brief Get the semaphore handle managed by this object.
         * \return Semaphore handle.
         */
        [[nodiscard]] const VkSemaphore& get() const noexcept;

    private:
        [[nodiscard]] static VkSemaphore createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan semaphore.
         */
        VkSemaphore semaphore = VK_NULL_HANDLE;
    };
}  // namespace sol