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
    class VulkanComputePipeline
    {
    public:
        /**
         * \brief VulkanComputePipeline settings.
         */
        struct Settings
        {
            /**
             * \brief Compute shader.
             */
            ObjectRefSetting<VulkanShaderModule> computeShader;

            ObjectRefListSetting<VulkanDescriptorSetLayout> descriptorSetLayouts;

            std::vector<VkPushConstantRange> pushConstants;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanComputePipeline() = delete;

        VulkanComputePipeline(Settings set, VkPipeline vkPipeline, VkPipelineLayout vkLayout);

        VulkanComputePipeline(const VulkanComputePipeline&) = delete;

        VulkanComputePipeline(VulkanComputePipeline&&) = delete;

        ~VulkanComputePipeline() noexcept;

        VulkanComputePipeline& operator=(const VulkanComputePipeline&) = delete;

        VulkanComputePipeline& operator=(VulkanComputePipeline&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan compute pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan compute pipeline.
         */
        [[nodiscard]] static VulkanComputePipelinePtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan compute pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan compute pipeline.
         */
        [[nodiscard]] static VulkanComputePipelineSharedPtr createShared(const Settings& settings);

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
         * \brief Vulkan compute pipeline.
         */
        VkPipeline pipeline = VK_NULL_HANDLE;

        /**
         * \brief Vulkan pipeline layout.
         */
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    };
}  // namespace sol
