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
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/material_layout.h"

namespace sol
{
    class ForwardMaterialLayout final : public MaterialLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct UniformBufferBinding
        {
            [[nodiscard]] bool operator==(const UniformBufferBinding& rhs) const noexcept;

            /**
             * \brief Optional user-friendly binding name.
             */
            std::string name;

            /**
             * \brief Set index.
             */
            uint32_t set = 0;

            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Size of the uniform buffer object (or a single buffer element if count > 1) in bytes.
             */
            size_t size = 0;

            /**
             * \brief Number of buffer elements.
             */
            uint32_t count = 1;

            /**
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;

            struct Usage
            {
                /**
                 * \brief Update detection method.
                 */
                UpdateDetection updateDetection = UpdateDetection::Always;

                /**
                 * \brief Expected update frequency.
                 */
                UpdateFrequency updateFrequency = UpdateFrequency::Frame;
            } usage;

            struct Sharing
            {
                /**
                 * \brief Sharing method.
                 */
                SharingMethod method = SharingMethod::None;

                /**
                 * \brief 
                 */
                size_t count = 1;
            } sharing;
        };

        struct SamplerBinding
        {
            [[nodiscard]] bool operator==(const SamplerBinding& rhs) const noexcept;

            /**
             * \brief Optional user-friendly binding name.
             */
            std::string name;

            /**
             * \brief Set index.
             */
            uint32_t set = 0;

            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Number of samplers.
             */
            uint32_t count = 1;

            /**
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;
        };

        struct PushConstant
        {
            [[nodiscard]] bool operator==(const PushConstant& rhs) const noexcept;

            /**
             * \brief Optional user-friendly push constant name.
             */
            std::string name;

            /**
             * \brief Offset of the range in bytes.
             */
            uint32_t offset = 0;

            /**
             * \brief Size of the range in bytes.
             */
            uint32_t size = 0;

            /**
             * \brief Stage(s) at which the range should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;
        };

        using UniformBufferBindingPtr = std::unique_ptr<UniformBufferBinding>;
        using SamplerBindingPtr       = std::unique_ptr<SamplerBinding>;
        using PushConstantPtr         = std::unique_ptr<PushConstant>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardMaterialLayout() = default;

        explicit ForwardMaterialLayout(VulkanDevice& vkDevice);

        ForwardMaterialLayout(const ForwardMaterialLayout&) = delete;

        ForwardMaterialLayout(ForwardMaterialLayout&&) = delete;

        ~ForwardMaterialLayout() override;

        ForwardMaterialLayout& operator=(const ForwardMaterialLayout&) = delete;

        ForwardMaterialLayout& operator=(ForwardMaterialLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool isFinalized() const noexcept;

        [[nodiscard]] const std::vector<UniformBufferBindingPtr>& getUniformBuffers() const noexcept;

        [[nodiscard]] const std::vector<SamplerBindingPtr>& getSamplers() const noexcept;

        [[nodiscard]] const std::vector<PushConstantPtr>& getPushConstants() const noexcept;

        [[nodiscard]] std::pair<const UniformBufferBindingPtr*, size_t> getUniformBuffers(uint32_t set) const;

        [[nodiscard]] std::pair<const SamplerBindingPtr*, size_t> getSamplers(uint32_t set) const;

        [[nodiscard]] const std::vector<VkPipelineColorBlendAttachmentState>& getColorBlending() const noexcept;

        /**
         * \brief Get the number of descriptor sets.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getSetCount() const noexcept;

        /**
         * \brief Get the number of uniform buffers.
         * \return Number of buffers.
         */
        [[nodiscard]] size_t getUniformBufferCount() const noexcept;

        /**
         * \brief Get the number of samplers.
         * \return Number of samplers.
         */
        [[nodiscard]] size_t getSamplerCount() const noexcept;

        /**
         * \brief Get the number of push constant ranges.
         * \return Number of ranges.
         */
        [[nodiscard]] size_t getPushConstantCount() const noexcept;

        /**
         * \brief Get the total size of the push constant ranges.
         * \return Total size in bytes.
         */
        [[nodiscard]] size_t getPushConstantTotalSize() const noexcept;

        /**
         * \brief Get the list of finalized VulkanDescriptorSetLayouts.
         * \throws SolError Thrown if layout was not yet finalized.
         * \return VulkanDescriptorSetLayouts.
         */
        [[nodiscard]] const std::vector<VulkanDescriptorSetLayoutPtr>& getFinalizedDescriptorSetLayouts() const;

        /**
         * \brief Get the list of finalized VkPushConstantRanges.
         * \throws SolError Thrown if layout was not yet finalized.
         * \return VkPushConstantRanges.
         */
        [[nodiscard]] const std::vector<VkPushConstantRange>& getFinalizedPushConstants() const;

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        UniformBufferBinding& addUniformBuffer();

        SamplerBinding& addSampler();

        PushConstant& addPushConstant();

        void addColorBlending(VkPipelineColorBlendAttachmentState state);

        /**
         * \brief Finalize this layout. The layout will no longer be modifiable. Will also sort all added objects by set and binding index.
         */
        void finalize();

        void requireFinalized() const;

        void requireNonFinalized() const;

        ////////////////////////////////////////////////////////////////
        // Compatibility.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Returns whether this and the other layout have fully compatible push constant ranges.
         * \param other Other layout.
         * \return True if compatible, false if not.
         */
        [[nodiscard]] bool isPushConstantCompatible(const ForwardMaterialLayout& other) const;

        /**
         * \brief Get the number of descriptor sets of this and the other layout that are compatible. Note: does not take into account push constant compatibility.
         * \param other Other layout.
         * \return Number of compatible descriptor sets. 0 means no sets.
         */
        [[nodiscard]] uint32_t getDescriptorSetCompatibility(const ForwardMaterialLayout& other) const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        /**
         * \brief List of uniform buffer bindings in this layout.
         */
        std::vector<UniformBufferBindingPtr> uniformBuffers;

        /**
         * \brief List of sampler bindings in this layout.
         */
        std::vector<SamplerBindingPtr> samplers;

        /**
         * \brief List of push constants in this layout.
         */
        std::vector<PushConstantPtr> pushConstants;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlending;

        bool finalized = false;

        /**
         * \brief Finalized descriptor set layouts.
         */
        std::vector<VulkanDescriptorSetLayoutPtr> finalLayouts;

        /**
         * \brief Finalized push constants.
         */
        std::vector<VkPushConstantRange> finalConstants;
    };
}  // namespace sol