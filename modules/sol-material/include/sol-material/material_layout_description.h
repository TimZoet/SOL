#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cstdint>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

namespace sol
{
    class MaterialLayoutDescription
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class UpdateDetection : uint32_t
        {
            /**
             * \brief Query material instance to see if data was updated.
             */
            Manual = 0,

            /**
             * \brief Automatically detect if the data changed by comparing it with the previous data.
             */
            Automatic = 1,

            /**
             * \brief Always update data, regardless of any changes.
             */
            Always = 2
        };

        enum class UpdateFrequency : uint32_t
        {
            /**
             * \brief Data is never updated after it is set for the first time.
             */
            Never = 0,

            /**
             * \brief Data is not expected to be updated often.
             */
            Low = 1,

            /**
             * \brief Data is expected to be updated at a high frequency, though not every frame.
             */
            High = 2,

            /**
             * \brief Data is expected to update every frame.
             */
            Frame = 3
        };

        enum class SharingMethod
        {
            /**
             * \brief Do not share any buffers.
             */
            None = 0,

            /**
             * \brief Share buffers for all bindings of a single material instance.
             */
            Instance = 1,

            /**
             * \brief Share buffers for the same binding of multiple material instances.
             */
            Binding = 2,

            /**
             * \brief Share buffers for multiple bindings of multiple material instances.
             */
            InstanceAndBinding = 3
        };

        struct AccelerationStructureBinding
        {
            [[nodiscard]] bool operator==(const AccelerationStructureBinding& rhs) const noexcept;

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
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;
        };

        struct CombinedImageSamplerBinding
        {
            [[nodiscard]] bool operator==(const CombinedImageSamplerBinding& rhs) const noexcept;

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

        struct StorageBufferBinding
        {
            [[nodiscard]] bool operator==(const StorageBufferBinding& rhs) const noexcept;

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
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;
        };

        struct StorageImageBinding
        {
            [[nodiscard]] bool operator==(const StorageImageBinding& rhs) const noexcept;

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
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;
        };

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

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MaterialLayoutDescription();

        MaterialLayoutDescription(const MaterialLayoutDescription&);

        MaterialLayoutDescription(MaterialLayoutDescription&&) noexcept;

        ~MaterialLayoutDescription() noexcept;

        MaterialLayoutDescription& operator=(const MaterialLayoutDescription&);

        MaterialLayoutDescription& operator=(MaterialLayoutDescription&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the number of descriptor sets.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getSetCount() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Compatibility.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the number of descriptor sets that are compatible between this and the other layout,
         * starting from the first descriptor set until the first incompatible set is reached.
         * \param other Other layout.
         * \return Number of compatible descriptor sets. 0 means no sets.
         */
        [[nodiscard]] uint32_t getDescriptorSetCompatibility(const MaterialLayoutDescription& other) const;

        /**
         * \brief Returns whether this and the other layout have fully compatible push constant ranges.
         * \param other Other layout.
         * \return True if compatible, false if not.
         */
        [[nodiscard]] bool isPushConstantCompatible(const MaterialLayoutDescription& other) const;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief List of acceleration structure bindings in this layout.
         */
        std::vector<AccelerationStructureBinding> accelerationStructures;

        /**
         * \brief List of combined image sampler bindings in this layout.
         */
        std::vector<CombinedImageSamplerBinding> combinedImageSamplers;

        /**
         * \brief List of storage buffer bindings in this layout.
         */
        std::vector<StorageBufferBinding> storageBuffers;

        /**
         * \brief List of storage image bindings in this layout.
         */
        std::vector<StorageImageBinding> storageImages;

        /**
         * \brief List of uniform buffer bindings in this layout.
         */
        std::vector<UniformBufferBinding> uniformBuffers;

        /**
         * \brief List of push constants in this layout.
         */
        std::vector<PushConstant> pushConstants;
    };
}  // namespace sol