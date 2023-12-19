#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

namespace sol
{
    class DescriptorLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct AccelerationStructureBinding
        {
            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Number of buffer acceleration structures.
             */
            uint32_t count = 1;

            /**
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;

            /**
             * \brief Memory offset of this binding in the layout. Calculated using vkGetDescriptorSetLayoutBindingOffsetEXT.
             */
            VkDeviceSize offset = 0;

            [[nodiscard]] bool operator==(const AccelerationStructureBinding& rhs) const noexcept
            {
                return binding == rhs.binding && count == rhs.count && stages == rhs.stages;
            }
        };

        struct SampledImageBinding
        {
            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Number of images.
             */
            uint32_t count = 1;

            /**
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;

            /**
             * \brief Memory offset of this binding in the layout. Calculated using vkGetDescriptorSetLayoutBindingOffsetEXT.
             */
            VkDeviceSize offset = 0;

            [[nodiscard]] bool operator==(const SampledImageBinding& rhs) const noexcept
            {
                return binding == rhs.binding && count == rhs.count && stages == rhs.stages;
            }
        };

        struct SamplerBinding
        {
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

            /**
             * \brief Memory offset of this binding in the layout. Calculated using vkGetDescriptorSetLayoutBindingOffsetEXT.
             */
            VkDeviceSize offset = 0;

            [[nodiscard]] bool operator==(const SamplerBinding& rhs) const noexcept
            {
                return binding == rhs.binding && count == rhs.count && stages == rhs.stages;
            }
        };

        struct StorageBufferBinding
        {
            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Number of buffer elements.
             */
            uint32_t count = 1;

            /**
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;

            /**
             * \brief Memory offset of this binding in the layout. Calculated using vkGetDescriptorSetLayoutBindingOffsetEXT.
             */
            VkDeviceSize offset = 0;

            [[nodiscard]] bool operator==(const StorageBufferBinding& rhs) const noexcept
            {
                return binding == rhs.binding && count == rhs.count && stages == rhs.stages;
            }
        };

        struct StorageImageBinding
        {
            /**
             * \brief Binding index.
             */
            uint32_t binding = 0;

            /**
             * \brief Number of images.
             */
            uint32_t count = 1;

            /**
             * \brief Stage(s) at which the binding should be accessible.
             */
            VkShaderStageFlags stages = VK_SHADER_STAGE_ALL;

            /**
             * \brief Memory offset of this binding in the layout. Calculated using vkGetDescriptorSetLayoutBindingOffsetEXT.
             */
            VkDeviceSize offset = 0;

            [[nodiscard]] bool operator==(const StorageImageBinding& rhs) const noexcept
            {
                return binding == rhs.binding && count == rhs.count && stages == rhs.stages;
            }
        };

        struct UniformBufferBinding
        {
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

            /**
             * \brief Memory offset of this binding in the layout. Calculated using vkGetDescriptorSetLayoutBindingOffsetEXT.
             */
            VkDeviceSize offset = 0;

            [[nodiscard]] bool operator==(const UniformBufferBinding& rhs) const noexcept
            {
                return binding == rhs.binding && size == rhs.size && count == rhs.count && stages == rhs.stages;
            }
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        DescriptorLayout() = delete;

        explicit DescriptorLayout(VulkanDevice& d);

        DescriptorLayout(const DescriptorLayout&) = delete;

        DescriptorLayout(DescriptorLayout&&) = delete;

        ~DescriptorLayout() noexcept;

        DescriptorLayout& operator=(const DescriptorLayout&) = delete;

        DescriptorLayout& operator=(DescriptorLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] size_t getLayoutSize() const;

        [[nodiscard]] const std::vector<AccelerationStructureBinding>& getAccelerationStructures() const noexcept;

        [[nodiscard]] const std::vector<SampledImageBinding>& getSampledImages() const noexcept;

        [[nodiscard]] const std::vector<SamplerBinding>& getSamplers() const noexcept;

        [[nodiscard]] const std::vector<StorageBufferBinding>& getStorageBuffers() const noexcept;

        [[nodiscard]] const std::vector<StorageImageBinding>& getStorageImages() const noexcept;

        [[nodiscard]] const std::vector<UniformBufferBinding>& getUniformBuffers() const noexcept;

        [[nodiscard]] const AccelerationStructureBinding& getAccelerationStructure(uint32_t binding) const;

        [[nodiscard]] const SampledImageBinding& getSampledImage(uint32_t binding) const;

        [[nodiscard]] const SamplerBinding& getSampler(uint32_t binding) const;

        [[nodiscard]] const StorageBufferBinding& getStorageBuffer(uint32_t binding) const;

        [[nodiscard]] const StorageImageBinding& getStorageImage(uint32_t binding) const;

        [[nodiscard]] const UniformBufferBinding& getUniformBuffer(uint32_t binding) const;

        ////////////////////////////////////////////////////////////////
        // Add.
        ////////////////////////////////////////////////////////////////

        void add(AccelerationStructureBinding binding);

        void add(SampledImageBinding binding);

        void add(SamplerBinding binding);

        void add(StorageBufferBinding binding);

        void add(StorageImageBinding binding);

        void add(UniformBufferBinding binding);

        ////////////////////////////////////////////////////////////////
        // Finalize.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Finalize this layout. The layout will no longer be modifiable.
         */
        void finalize();

        void requireFinalized() const;

        void requireNonFinalized() const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Vulkan device.
         */
        VulkanDevice* device = nullptr;

        /**
         * \brief List of acceleration structure bindings in this layout.
         */
        std::vector<AccelerationStructureBinding> accelerationStructures;

        /**
         * \brief List of sampler bindings in this layout.
         */
        std::vector<SampledImageBinding> sampledImages;

        /**
         * \brief List of sampler bindings in this layout.
         */
        std::vector<SamplerBinding> samplers;

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
         * \brief Finalized layout.
         */
        VulkanDescriptorSetLayoutPtr layout;

        /**
         * \brief Aligned size of layout in bytes.
         */
        size_t layoutSize = 0;
    };
}  // namespace sol
