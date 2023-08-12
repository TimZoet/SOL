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
    class VulkanTimelineSemaphore
    {
    public:
        /**
         * \brief VulkanTimelineSemaphore settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            uint64_t initialValue = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanTimelineSemaphore() = delete;

        VulkanTimelineSemaphore(const Settings& set, VkSemaphore vkSemaphore);

        VulkanTimelineSemaphore(const VulkanTimelineSemaphore&) = delete;

        VulkanTimelineSemaphore(VulkanTimelineSemaphore&&) = delete;

        ~VulkanTimelineSemaphore() noexcept;

        VulkanTimelineSemaphore& operator=(const VulkanTimelineSemaphore&) = delete;

        VulkanTimelineSemaphore& operator=(VulkanTimelineSemaphore&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan timeline semaphore.
         * \param settings Settings.
         * \throws VulkanError Thrown if timeline semaphore creation failed.
         * \return Vulkan timeline semaphore.
         */
        [[nodiscard]] static VulkanTimelineSemaphorePtr create(const Settings& settings);

        /**
         * \brief Create a list of new Vulkan timeline semaphores.
         * \param settings Settings.
         * \param count Number of timeline semaphores to create.
         * \throws VulkanError Thrown if timeline semaphore creation failed.
         * \return List of Vulkan timeline semaphores.
         */
        [[nodiscard]] static std::vector<VulkanTimelineSemaphorePtr> create(Settings settings, size_t count);

        /**
         * \brief Create a new Vulkan timeline semaphore.
         * \param settings Settings.
         * \throws VulkanError Thrown if timeline semaphore creation failed.
         * \return Vulkan timeline semaphore.
         */
        [[nodiscard]] static VulkanTimelineSemaphoreSharedPtr createShared(const Settings& settings);

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
         * \brief Get the semaphore handle managed by this object.
         * \return Semaphore handle.
         */
        [[nodiscard]] const VkSemaphore& get() const noexcept;

    private:
        [[nodiscard]] static VkSemaphore createImpl(const Settings& settings);

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanDevice* device = nullptr;
#endif

        /**
         * \brief Vulkan semaphore.
         */
        VkSemaphore semaphore = VK_NULL_HANDLE;
    };
}  // namespace sol
