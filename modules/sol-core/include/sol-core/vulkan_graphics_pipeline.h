#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
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
             * \brief Vertex shader.
             */
            ObjectRefSetting<VulkanShaderModule> vertexShader;

            /**
             * \brief Fragment shader.
             */
            ObjectRefSetting<VulkanShaderModule> fragmentShader;

            std::vector<VkVertexInputAttributeDescription> vertexAttributes;

            std::vector<VkVertexInputBindingDescription> vertexBindings;

            ObjectRefListSetting<VulkanDescriptorSetLayout> descriptorSetLayouts;

            std::vector<VkPushConstantRange> pushConstants;

            std::vector<VkPipelineColorBlendAttachmentState> colorBlending;

            std::optional<VkPipelineRasterizationStateCreateInfo> rasterization;

            std::optional<VkPipelineDepthStencilStateCreateInfo> depthStencil;
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