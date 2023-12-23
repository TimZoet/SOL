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
    class VulkanGraphicsPipelinePreRasterization
    {
    public:
        /**
         * \brief VulkanGraphicsPipelinePreRasterization settings.
         */
        struct Settings
        {
            ObjectRefSetting<VulkanPipelineLayout> layout;

            struct Shader
            {
                VkPipelineShaderStageCreateFlags stageFlags = 0;

                VkShaderModuleCreateFlags moduleFlags = 0;

                /**
                 * \brief Entry point name.
                 */
                std::string entrypoint = "main";

                /**
                 * \brief If not empty, this shader stage is enabled.
                 */
                std::vector<std::byte> code;
            };

            Shader vertexShader;

            Shader tessellationControlShader;

            Shader tessellationEvaluationShader;

            Shader geometryShader;

            struct
            {
                VkPipelineViewportStateCreateFlags flags = 0;

                /**
                 * \brief List of viewports. Ignored if this state is dynamic.
                 */
                std::vector<VkViewport> viewports;

                /**
                 * \brief List of scissors. Ignored if this state is dynamic.
                 */
                std::vector<VkRect2D> scissors;
            } viewport;

            struct
            {
                bool depthClampEnable = false;

                bool rasterizerDiscardEnable = false;

                VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;

                VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;

                VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

                bool depthBiasEnable = false;

                float depthBiasConstantFactor = 0.0f;

                float depthBiasClamp = 0.0f;

                float depthBiasSlopeFactor = 0.0f;

                float lineWidth = 1.0f;
            } rasterization;

            struct
            {
                uint32_t patchControlPoints = 0;
            } tessellation;

            /**
             * \brief List of all enabled dynamic states.
             */
            std::vector<VkDynamicState> enabledDynamicStates;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipelinePreRasterization() = delete;

        VulkanGraphicsPipelinePreRasterization(Settings set, VkPipeline vkPipeline);

        VulkanGraphicsPipelinePreRasterization(const VulkanGraphicsPipelinePreRasterization&) = delete;

        VulkanGraphicsPipelinePreRasterization(VulkanGraphicsPipelinePreRasterization&&) = delete;

        ~VulkanGraphicsPipelinePreRasterization() noexcept;

        VulkanGraphicsPipelinePreRasterization& operator=(const VulkanGraphicsPipelinePreRasterization&) = delete;

        VulkanGraphicsPipelinePreRasterization& operator=(VulkanGraphicsPipelinePreRasterization&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelinePreRasterizationPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelinePreRasterizationSharedPtr createShared(const Settings& settings);

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