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
    class VulkanDescriptorPool
    {
    public:
        /**
         * \brief VulkanDescriptorPool settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Maximum number of descriptor sets that can be allocated from this pool.
             */
            uint32_t maxSets = 0;

            /**
             * \brief Number of descriptor sets that should be allocated for each descriptor type.
             */
            std::vector<VkDescriptorPoolSize> poolSizes;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanDescriptorPool() = delete;

        VulkanDescriptorPool(SettingsPtr settingsPtr, VkDescriptorPool vkPool);

        VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;

        VulkanDescriptorPool(VulkanDescriptorPool&&) = delete;

        ~VulkanDescriptorPool() noexcept;

        VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

        VulkanDescriptorPool& operator=(VulkanDescriptorPool&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan descriptor pool.
         * \param settings Settings.
         * \throws VulkanError Thrown if pool creation failed.
         * \return Vulkan descriptor pool.
         */
        [[nodiscard]] static VulkanDescriptorPoolPtr create(Settings settings);

        /**
         * \brief  Create a new Vulkan descriptor pool.
         * \param settings Settings.
         * \throws VulkanError Thrown if pool creation failed.
         * \return Vulkan descriptor pool.
         */
        [[nodiscard]] static VulkanDescriptorPoolSharedPtr createShared(Settings settings);

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
         * \brief Get the pool handle managed by this object.
         * \return Pool handle.
         */
        [[nodiscard]] const VkDescriptorPool& get() const noexcept;

    private:
        [[nodiscard]] static VkDescriptorPool createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan descriptor pool.
         */
        VkDescriptorPool pool = VK_NULL_HANDLE;
    };
}  // namespace sol