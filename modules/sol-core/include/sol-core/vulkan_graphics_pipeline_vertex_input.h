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
    class VulkanGraphicsPipelineVertexInput
    {
    public:
        /**
         * \brief VulkanGraphicsPipelineVertexInput settings.
         */
        struct Settings
        {
            ObjectRefSetting<VulkanDevice> device;

            std::vector<VkVertexInputAttributeDescription> vertexAttributes;

            std::vector<VkVertexInputBindingDescription> vertexBindings;

            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

            bool primitiveRestartEnable = false;

            /**
             * \brief List of all enabled dynamic states.
             */
            std::vector<VkDynamicState> enabledDynamicStates;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipelineVertexInput() = delete;

        VulkanGraphicsPipelineVertexInput(Settings set, VkPipeline vkPipeline);

        VulkanGraphicsPipelineVertexInput(const VulkanGraphicsPipelineVertexInput&) = delete;

        VulkanGraphicsPipelineVertexInput(VulkanGraphicsPipelineVertexInput&&) = delete;

        ~VulkanGraphicsPipelineVertexInput() noexcept;

        VulkanGraphicsPipelineVertexInput& operator=(const VulkanGraphicsPipelineVertexInput&) = delete;

        VulkanGraphicsPipelineVertexInput& operator=(VulkanGraphicsPipelineVertexInput&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelineVertexInputPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelineVertexInputSharedPtr createShared(const Settings& settings);

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
         * \brief Get the pipeline handle managed by this object.
         * \return Pipeline handle.
         */
        [[nodiscard]] const VkPipeline& get() const noexcept;

        /**
         * \brief Get the list of enabled dynamic states.
         * \return List of dynamic states.
         */
        [[nodiscard]] const std::vector<VkDynamicState>& getDynamicStates() const noexcept;

    private:
        [[nodiscard]] static VkPipeline createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;

        /**
         * \brief Vulkan graphics pipeline.
         */
        VkPipeline pipeline = VK_NULL_HANDLE;
    };
}  // namespace sol