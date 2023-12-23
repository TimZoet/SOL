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
    class VulkanGraphicsPipelineFragment
    {
    public:
        /**
         * \brief VulkanGraphicsPipelineFragment settings.
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

            Shader fragmentShader;

            struct
            {
                VkPipelineDepthStencilStateCreateFlags flags = 0;

                bool depthTestEnable = true;

                bool depthWriteEnable = true;

                VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;

                bool depthBoundsTestEnable = false;

                bool stencilTestEnable = false;

                VkStencilOpState front{};

                VkStencilOpState back{};

                float minDepthBounds = 0;

                float maxDepthBounds = 0;

            } depthStencil;
            /**
             * \brief List of all enabled dynamic states.
             */
            std::vector<VkDynamicState> enabledDynamicStates;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipelineFragment() = delete;

        VulkanGraphicsPipelineFragment(Settings set, VkPipeline vkPipeline);

        VulkanGraphicsPipelineFragment(const VulkanGraphicsPipelineFragment&) = delete;

        VulkanGraphicsPipelineFragment(VulkanGraphicsPipelineFragment&&) = delete;

        ~VulkanGraphicsPipelineFragment() noexcept;

        VulkanGraphicsPipelineFragment& operator=(const VulkanGraphicsPipelineFragment&) = delete;

        VulkanGraphicsPipelineFragment& operator=(VulkanGraphicsPipelineFragment&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelineFragmentPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan graphics pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan graphics pipeline.
         */
        [[nodiscard]] static VulkanGraphicsPipelineFragmentSharedPtr createShared(const Settings& settings);

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