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
    class VulkanDescriptorSetLayout
    {
    public:
        /**
         * \brief VulkanDescriptorSetLayout settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Bindings.
             */
            std::vector<VkDescriptorSetLayoutBinding> bindings;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanDescriptorSetLayout() = delete;

        VulkanDescriptorSetLayout(SettingsPtr settingsPtr, VkDescriptorSetLayout vkLayout);

        VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;

        VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&&) = delete;

        ~VulkanDescriptorSetLayout() noexcept;

        VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

        VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan descriptor set layout.
         * \param settings Settings.
         * \throws VulkanError Thrown if layout creation failed.
         * \return Vulkan descriptor set layout.
         */
        [[nodiscard]] static VulkanDescriptorSetLayoutPtr create(Settings settings);

        /**
         * \brief Create a new Vulkan descriptor set layout.
         * \param settings Settings.
         * \throws VulkanError Thrown if layout creation failed.
         * \return Vulkan descriptor set layout.
         */
        [[nodiscard]] static VulkanDescriptorSetLayoutSharedPtr createShared(Settings settings);

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
         * \brief Get the layout handle managed by this object.
         * \return Layout handle.
         */
        [[nodiscard]] const VkDescriptorSetLayout& get() const noexcept;

    private:
        [[nodiscard]] static VkDescriptorSetLayout createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan descriptor set layout.
         */
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    };
}  // namespace sol