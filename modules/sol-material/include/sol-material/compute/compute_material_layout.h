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
    class ComputeMaterialLayout final : public MaterialLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

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

        using StorageImageBindingPtr  = std::unique_ptr<StorageImageBinding>;
        using StorageBufferBindingPtr = std::unique_ptr<StorageBufferBinding>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeMaterialLayout() = default;

        explicit ComputeMaterialLayout(VulkanDevice& vkDevice);

        ComputeMaterialLayout(const ComputeMaterialLayout&) = delete;

        ComputeMaterialLayout(ComputeMaterialLayout&&) = delete;

        ~ComputeMaterialLayout() override;

        ComputeMaterialLayout& operator=(const ComputeMaterialLayout&) = delete;

        ComputeMaterialLayout& operator=(ComputeMaterialLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool isFinalized() const noexcept;

        [[nodiscard]] const std::vector<StorageImageBindingPtr>& getStorageImages() const noexcept;

        [[nodiscard]] std::pair<const StorageImageBindingPtr*, size_t> getStorageImages(uint32_t set) const;

        [[nodiscard]] const std::vector<StorageBufferBindingPtr>& getStorageBuffers() const noexcept;

        [[nodiscard]] std::pair<const StorageBufferBindingPtr*, size_t> getStorageBuffers(uint32_t set) const;

        /**
         * \brief Get the number of descriptor sets.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getSetCount() const noexcept;

        /**
         * \brief Get the number of storage images.
         * \return Number of images.
         */
        [[nodiscard]] size_t getStorageImageCount() const noexcept;

        /**
         * \brief Get the number of storage buffers.
         * \return Number of buffers.
         */
        [[nodiscard]] size_t getStorageBufferCount() const noexcept;

        /**
         * \brief Get the list of finalized VulkanDescriptorSetLayouts.
         * \throws SolError Thrown if layout was not yet finalized.
         * \return VulkanDescriptorSetLayouts.
         */
        [[nodiscard]] const std::vector<VulkanDescriptorSetLayoutPtr>& getFinalizedDescriptorSetLayouts() const;

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        StorageImageBinding& addStorageImage();

        StorageBufferBinding& addStorageBuffer();

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
         * \brief Get the number of descriptor sets of this and the other layout that are compatible. Note: does not take into account push constant compatibility.
         * \param other Other layout.
         * \return Number of compatible descriptor sets. 0 means no sets.
         */
        [[nodiscard]] uint32_t getDescriptorSetCompatibility(const ComputeMaterialLayout& other) const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        /**
         * \brief List of storage image bindings in this layout.
         */
        std::vector<StorageImageBindingPtr> storageImages;

        /**
         * \brief List of storage buffer bindings in this layout.
         */
        std::vector<StorageBufferBindingPtr> storageBuffers;

        bool finalized = false;

        /**
         * \brief Finalized descriptor set layouts.
         */
        std::vector<VulkanDescriptorSetLayoutPtr> finalLayouts;
    };
}  // namespace sol
