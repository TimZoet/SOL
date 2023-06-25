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
        struct HitShaderGroup
        {
            /**
             * \brief Index into the closestHitShaders list.
             */
            std::optional<uint32_t> closest;

            /**
             * \brief Index into the anyHitShaders list.
             */
            std::optional<uint32_t> any;

            /**
             * \brief Index into the intersectionShaders list.
             */
            std::optional<uint32_t> intersection;
        };

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
             * \brief Optional explicit entry point name for raygen shader. If empty, defaults to "main".
             */
            std::string raygenEntryPoint;

            /**
             * \brief Miss shaders.
             */
            ObjectRefListSetting<VulkanShaderModule> missShaders;

            /**
             * \brief Optional explicit entry point names for miss shaders. If empty, defaults to "main".
             */
            std::vector<std::string> missEntryPoints;

            /**
             * \brief Closest hit shaders.
             */
            ObjectRefListSetting<VulkanShaderModule> closestHitShaders;

            /**
             * \brief Optional explicit entry point names for closest hit shaders. If empty, defaults to "main".
             */
            std::vector<std::string> closestHitEntryPoints;

            /**
             * \brief Any hit shaders.
             */
            ObjectRefListSetting<VulkanShaderModule> anyHitShaders;

            /**
             * \brief Optional explicit entry point names for any hit shaders. If empty, defaults to "main".
             */
            std::vector<std::string> anyHitEntryPoints;

            /**
             * \brief Intersection shaders.
             */
            ObjectRefListSetting<VulkanShaderModule> intersectionShaders;

            /**
             * \brief Optional explicit entry point names for intersection shaders. If empty, defaults to "main".
             */
            std::vector<std::string> intersectionEntryPoints;

            /**
             * \brief Callable shaders.
             */
            ObjectRefListSetting<VulkanShaderModule> callableShaders;

            /**
             * \brief Optional explicit entry point names for callable shaders. If empty, defaults to "main".
             */
            std::vector<std::string> callableEntryPoints;

            /**
             * \brief List of hit groups.
             */
            std::vector<HitShaderGroup> hitGroups;

            /**
             * \brief List of descriptor set layouts.
             */
            ObjectRefListSetting<VulkanDescriptorSetLayout> descriptorSetLayouts;

            /**
             * \brief List of push constant ranges.
             */
            std::vector<VkPushConstantRange> pushConstants;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanRayTracingPipeline() = delete;

        VulkanRayTracingPipeline(Settings set, VkPipeline vkPipeline, VkPipelineLayout vkLayout);

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
        [[nodiscard]] const VkPipeline& get() const noexcept;

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

        /**
         * \brief Get the number of raygen shader groups.
         * \return Group count.
         */
        [[nodiscard]] uint32_t getRaygenGroupCount() const noexcept;

        /**
         * \brief Get the number of miss shader groups.
         * \return Group count.
         */
        [[nodiscard]] uint32_t getMissGroupCount() const noexcept;

        /**
         * \brief Get the number of hit shader groups.
         * \return Group count.
         */
        [[nodiscard]] uint32_t getHitGroupCount() const noexcept;

        /**
         * \brief Get the number of callable shader groups.
         * \return Group count.
         */
        [[nodiscard]] uint32_t getCallableGroupCount() const noexcept;

    private:
        [[nodiscard]] static std::tuple<VkPipeline, VkPipelineLayout> createImpl(const Settings& settings);

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
    };
}  // namespace sol