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
    class VulkanGraphicsPipeline2
    {
    public:
        /**
         * \brief VulkanGraphicsPipeline2 settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Vertex input pipeline to link.
             */
            ObjectRefSetting<VulkanGraphicsPipelineVertexInput> vertexInputPipeline;

            /**
             * \brief Pre-rasterization pipeline to link.
             */
            ObjectRefSetting<VulkanGraphicsPipelinePreRasterization> preRasterizationPipeline;

            /**
             * \brief Fragment pipeline to link.
             */
            ObjectRefSetting<VulkanGraphicsPipelineFragment> fragmentPipeline;

            /**
             * \brief Fragment output pipeline to link.
             */
            ObjectRefSetting<VulkanGraphicsPipelineFragmentOutput> fragmentOutputPipeline;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipeline2() = delete;

        VulkanGraphicsPipeline2(Settings set, VkPipeline vkPipeline);

        VulkanGraphicsPipeline2(const VulkanGraphicsPipeline2&) = delete;

        VulkanGraphicsPipeline2(VulkanGraphicsPipeline2&&) = delete;

        ~VulkanGraphicsPipeline2() noexcept;

        VulkanGraphicsPipeline2& operator=(const VulkanGraphicsPipeline2&) = delete;

        VulkanGraphicsPipeline2& operator=(VulkanGraphicsPipeline2&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipeline2Ptr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipeline2SharedPtr createShared(const Settings& settings);

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