#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <utility>
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
    class VulkanGraphicsPipeline
    {
    public:
        /**
         * \brief VulkanGraphicsPipeline settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan render pass.
             */
            ObjectRefSetting<VulkanRenderPass> renderPass;

            /**
             * \brief Vertex shader module.
             */
            ObjectRefSetting<VulkanShaderModule> vertexShader;

            /**
             * \brief Vertex shader entry point.
             */
            std::string vertexEntryPoint = "main";

            /**
             * \brief Fragment shader module.
             */
            ObjectRefSetting<VulkanShaderModule> fragmentShader;

            /**
             * \brief Fragment shader entry point.
             */
            std::string fragmentEntryPoint = "main";

            std::vector<VkVertexInputAttributeDescription> vertexAttributes;

            std::vector<VkVertexInputBindingDescription> vertexBindings;

            ObjectRefListSetting<VulkanDescriptorSetLayout> descriptorSetLayouts;

            std::vector<VkPushConstantRange> pushConstants;

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{
              .sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
              .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};

            VkPipelineViewportStateCreateInfo viewport{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};

            /**
             * \brief If not empty, this list will override the pViewports of the viewport create info.
             */
            std::vector<VkViewport> viewports;

            /**
             * \brief If not empty, this list will override the pScissors of the viewport create info.
             */
            std::vector<VkRect2D> scissors;

            VkPipelineRasterizationStateCreateInfo rasterization{
              .sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
              .polygonMode = VK_POLYGON_MODE_FILL,
              .cullMode    = VK_CULL_MODE_BACK_BIT,
              .frontFace   = VK_FRONT_FACE_COUNTER_CLOCKWISE,
              .lineWidth   = 1.0f};

            VkPipelineMultisampleStateCreateInfo multisample{.sType =
                                                               VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                                                             .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT};

            VkPipelineDepthStencilStateCreateInfo depthStencil{
              .sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
              .depthTestEnable  = VK_TRUE,
              .depthWriteEnable = VK_TRUE,
              .depthCompareOp   = VK_COMPARE_OP_LESS};

            VkPipelineColorBlendStateCreateInfo colorBlend{.sType =
                                                             VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};

            /**
             * \brief If not empty, this list will override the pAttachments of the color blend create info.
             */
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

            std::vector<VkDynamicState> enabledDynamicStates;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipeline() = delete;

        VulkanGraphicsPipeline(Settings set, VkPipeline vkPipeline, VkPipelineLayout vkLayout);

        VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;

        VulkanGraphicsPipeline(VulkanGraphicsPipeline&&) = delete;

        ~VulkanGraphicsPipeline() noexcept;

        VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;

        VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelinePtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelineSharedPtr createShared(const Settings& settings);

        void destroy();

        void recreate();

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
        [[nodiscard]] const VkPipeline& getPipeline() const noexcept;

        /**
         * \brief Get the pipeline layout handle managed by this object.
         * \return Pipeline layout handle.
         */
        [[nodiscard]] const VkPipelineLayout& getPipelineLayout() const noexcept;

    private:
        [[nodiscard]] static std::pair<VkPipeline, VkPipelineLayout> createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;

        /**
         * \brief Vulkan graphics pipeline.
         */
        VkPipeline pipeline = VK_NULL_HANDLE;

        /**
         * \brief Vulkan pipeline layout.
         */
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    };
}  // namespace sol