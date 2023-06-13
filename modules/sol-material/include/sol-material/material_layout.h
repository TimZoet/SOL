#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <span>

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

#include "sol-material/material_layout_description.h"

namespace sol
{
    class MaterialLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MaterialLayout() = delete;

        explicit MaterialLayout(VulkanDevice& device);

        MaterialLayout(const MaterialLayout&) = delete;

        MaterialLayout(MaterialLayout&&) = delete;

        virtual ~MaterialLayout() noexcept;

        MaterialLayout& operator=(const MaterialLayout&) = delete;

        MaterialLayout& operator=(MaterialLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool isFinalized() const noexcept;

        [[nodiscard]] const MaterialLayoutDescription& getDescription() const;

        /**
         * \brief Get the number of descriptor sets.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getSetCount() const noexcept;

        /**
         * \brief Get the list of all acceleration structure bindings for the given set.
         * \param set Descriptor set index.
         * \return List of bindings.
         */
        [[nodiscard]] std::span<const MaterialLayoutDescription::AccelerationStructureBinding>
          getAccelerationStructures(uint32_t set) const;

        /**
         * \brief Get the list of all combined image sampler bindings for the given set.
         * \param set Descriptor set index.
         * \return List of bindings.
         */
        [[nodiscard]] std::span<const MaterialLayoutDescription::CombinedImageSamplerBinding>
          getCombinedImageSamplers(uint32_t set) const;

        /**
         * \brief Get the list of all storage buffer bindings for the given set.
         * \param set Descriptor set index.
         * \return List of bindings.
         */
        [[nodiscard]] std::span<const MaterialLayoutDescription::StorageBufferBinding>
          getStorageBuffers(uint32_t set) const;

        /**
         * \brief Get the list of all image buffer bindings for the given set.
         * \param set Descriptor set index.
         * \return List of bindings.
         */
        [[nodiscard]] std::span<const MaterialLayoutDescription::StorageImageBinding>
          getStorageImages(uint32_t set) const;

        /**
         * \brief Get the list of all uniform buffer bindings for the given set.
         * \param set Descriptor set index.
         * \return List of bindings.
         */
        [[nodiscard]] std::span<const MaterialLayoutDescription::UniformBufferBinding>
          getUniformBuffers(uint32_t set) const;

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
        [[nodiscard]] const std::vector<VulkanDescriptorSetLayoutPtr>& getDescriptorSetLayouts() const;

        /**
         * \brief Get a finalized VulkanDescriptorSetLayout by set index.
         * \param set Descriptor set index.
         * \return VulkanDescriptorSetLayout.
         */
        [[nodiscard]] const VulkanDescriptorSetLayout& getDescriptorSetLayout(size_t set) const;

        /**
         * \brief Get the list of finalized VkPushConstantRanges.
         * \throws SolError Thrown if layout was not yet finalized.
         * \return VkPushConstantRanges.
         */
        [[nodiscard]] const std::vector<VkPushConstantRange>& getPushConstants() const;

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Finalize this layout. The layout will no longer be modifiable.
         * \param desc Layout description.
         */
        virtual void finalize(MaterialLayoutDescription desc);

        void requireFinalized() const;

        void requireNonFinalized() const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        MaterialLayoutDescription description;

        bool finalized = false;

        /**
         * \brief Finalized descriptor set layouts.
         */
        std::vector<VulkanDescriptorSetLayoutPtr> layouts;

        /**
         * \brief Finalized push constants.
         */
        std::vector<VkPushConstantRange> pushConstants;
    };
}  // namespace sol