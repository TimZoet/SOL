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
    class VulkanPipelineLayout
    {
    public:
        /**
         * \brief VulkanPipelineLayout settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief List of descriptor set layouts.
             */
            ObjectRefListSetting<VulkanDescriptorSetLayout> descriptors;

            /**
             * \brief Enable independent descriptor set layouts.
             */
            bool independentDescriptors = false;

            /**
             * \brief List of push constants.
             */
            std::vector<VkPushConstantRange> pushConstants;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanPipelineLayout() = delete;

        VulkanPipelineLayout(Settings set, VkPipelineLayout vkLayout);

        VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;

        VulkanPipelineLayout(VulkanPipelineLayout&&) = delete;

        ~VulkanPipelineLayout() noexcept;

        VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;

        VulkanPipelineLayout& operator=(VulkanPipelineLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan pipeline layout.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline layout creation failed.
         * \return Vulkan pipeline layout.
         */
        [[nodiscard]] static VulkanPipelineLayoutPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan pipeline layout.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline layout creation failed.
         * \return Vulkan pipeline layout.
         */
        [[nodiscard]] static VulkanPipelineLayoutSharedPtr createShared(const Settings& settings);

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
         * \brief Get the pipeline layout handle managed by this object.
         * \return Pipeline layout handle.
         */
        [[nodiscard]] const VkPipelineLayout& get() const noexcept;

    private:
        [[nodiscard]] static VkPipelineLayout createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;

        /**
         * \brief Vulkan pipeline layout.
         */
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    };
}  // namespace sol