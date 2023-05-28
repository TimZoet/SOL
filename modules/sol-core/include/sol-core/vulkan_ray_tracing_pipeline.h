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
    class VulkanRayTracingPipeline
    {
    public:
        /**
         * \brief VulkanRayTracingPipeline settings.
         */
        struct Settings
        {
            /**
             * \brief Raygen shader.
             */
            ObjectRefSetting<VulkanShaderModule> raygenShader;

            /**
             * \brief Miss shader.
             */
            ObjectRefSetting<VulkanShaderModule> missShader;

            /**
             * \brief Closest hit shader.
             */
            ObjectRefSetting<VulkanShaderModule> closestHitShader;

            /**
             * \brief Any hit shader.
             */
            ObjectRefSetting<VulkanShaderModule> anyHitShader;

            /**
             * \brief Intersection shader.
             */
            ObjectRefSetting<VulkanShaderModule> intersectionShader;

            // TODO: No callables yet.

            ObjectRefListSetting<VulkanDescriptorSetLayout> descriptorSetLayouts;

            std::vector<VkPushConstantRange> pushConstants;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanRayTracingPipeline() = delete;

        VulkanRayTracingPipeline(Settings set, VkPipeline vkPipeline, VkPipelineLayout vkLayout, uint32_t sGroupCount);

        VulkanRayTracingPipeline(const VulkanRayTracingPipeline&) = delete;

        VulkanRayTracingPipeline(VulkanRayTracingPipeline&&) = delete;

        ~VulkanRayTracingPipeline() noexcept;

        VulkanRayTracingPipeline& operator=(const VulkanRayTracingPipeline&) = delete;

        VulkanRayTracingPipeline& operator=(VulkanRayTracingPipeline&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan ray tracing pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan ray tracing pipeline.
         */
        [[nodiscard]] static VulkanRayTracingPipelinePtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan ray tracing pipeline.
         * \param settings Settings.
         * \throws VulkanError Thrown if pipeline creation failed.
         * \return Vulkan ray tracing pipeline.
         */
        [[nodiscard]] static VulkanRayTracingPipelineSharedPtr createShared(const Settings& settings);

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

        [[nodiscard]] uint32_t getShaderGroupCount() const noexcept;

    private:
        [[nodiscard]] static std::tuple<VkPipeline, VkPipelineLayout, uint32_t> createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;

        /**
         * \brief Vulkan ray tracing pipeline.
         */
        VkPipeline pipeline = VK_NULL_HANDLE;

        /**
         * \brief Vulkan pipeline layout.
         */
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

        uint32_t shaderGroupCount = 0;
    };
}  // namespace sol