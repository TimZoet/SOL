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
    class VulkanGraphicsPipelineFragmentOutput
    {
    public:
        /**
         * \brief VulkanGraphicsPipelineFragmentOutput settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Multisampling.
             */
            struct
            {
                VkPipelineMultisampleStateCreateFlags flags = 0;

                VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

                bool sampleShadingEnable = false;

                float minSampleShading = 1.0f;

                std::vector<VkSampleMask> sampleMasks;

                bool alphaToCoverageEnable = false;

                bool alphaToOneEnable = false;
            } multisample;

            /**
             * \brief Color blending.
             */
            struct
            {
                VkPipelineColorBlendStateCreateFlags flags = 0;

                VkBool32 logicOpEnable = false;

                VkLogicOp logicOp = VK_LOGIC_OP_CLEAR;

                std::vector<VkPipelineColorBlendAttachmentState> attachments;
            } colorBlend;

            /**
             * \brief List of color attachment formats.
             */
            std::vector<VkFormat> colorAttachmentFormats;

            /**
             * \brief Depth attachment format.
             */
            VkFormat depthAttachmentFormat = VK_FORMAT_UNDEFINED;

            /**
             * \brief Stencil attachment format.
             */
            VkFormat stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

            /**
             * \brief List of all enabled dynamic states.
             */
            std::vector<VkDynamicState> enabledDynamicStates;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipelineFragmentOutput() = delete;

        VulkanGraphicsPipelineFragmentOutput(Settings set, VkPipeline vkPipeline);

        VulkanGraphicsPipelineFragmentOutput(const VulkanGraphicsPipelineFragmentOutput&) = delete;

        VulkanGraphicsPipelineFragmentOutput(VulkanGraphicsPipelineFragmentOutput&&) = delete;

        ~VulkanGraphicsPipelineFragmentOutput() noexcept;

        VulkanGraphicsPipelineFragmentOutput& operator=(const VulkanGraphicsPipelineFragmentOutput&) = delete;

        VulkanGraphicsPipelineFragmentOutput& operator=(VulkanGraphicsPipelineFragmentOutput&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelineFragmentOutputPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelineFragmentOutputSharedPtr createShared(const Settings& settings);

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