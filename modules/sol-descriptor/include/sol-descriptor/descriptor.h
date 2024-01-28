#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <span>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-core/vulkan_top_level_acceleration_structure.h"
#include "sol-error/sol_error.h"
#include "sol-memory/i_buffer.h"
#include "sol-texture/sampler2d.h"
#include "sol-texture/texture2d2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/descriptor_buffer.h"
#include "sol-descriptor/descriptor_layout.h"

namespace sol
{
    class Descriptor
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class DescType
        {
            AccelerationStructure,
            SampledImage,
            Sampler,
            StorageBuffer,
            StorageImage,
            UniformBuffer
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Descriptor() = delete;

        Descriptor(DescriptorBuffer&       buffer,
                   const DescriptorLayout& layout,
                   VmaVirtualAllocation    allocation,
                   VkDeviceSize            offset);

        Descriptor(const Descriptor&) = delete;

        Descriptor(Descriptor&&) = delete;

        ~Descriptor() noexcept;

        Descriptor& operator=(const Descriptor&) = delete;

        Descriptor& operator=(Descriptor&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] DescriptorBuffer& getBuffer();

        [[nodiscard]] const DescriptorBuffer& getBuffer() const;

        [[nodiscard]] const DescriptorLayout& getLayout() const;

        /**
         * \brief Get the offset of the descriptor into the descriptor buffer.
         * \return Offset  in bytes.
         */
        [[nodiscard]] VkDeviceSize getOffset() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        // TODO: How to clear descriptors?

        /**
         * \brief Update single acceleration structure at index.
         * \param desc Acceleration structure.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setAccelerationStructure(const VulkanTopLevelAccelerationStructure& desc,
                                      uint32_t                                   binding,
                                      uint32_t                                   index) const;

        /**
         * \brief Update single sampled image at index.
         * \param desc Image.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setSampledImage(const VulkanImageView& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single sampled image at index.
         * \param desc Image.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setSampledImage(const Texture2D2& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single sampler at index.
         * \param desc Sampler.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setSampler(const VulkanSampler& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single sampler at index.
         * \param desc Sampler.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setSampler(const Sampler2D& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single sampler at index.
         * \param desc Sampler.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setSampler(const Texture2D2& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single storage buffer at index.
         * \param desc Buffer.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setStorageBuffer(const IBuffer& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single storage image at index.
         * \param desc Image.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setStorageImage(const VulkanImageView& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single storage image at index.
         * \param desc Image.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setStorageImage(const Texture2D2& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update single uniform buffer at index.
         * \param desc Buffer.
         * \param binding Binding index.
         * \param index Index into binding, if it is an array.
         */
        void setUniformBuffer(const IBuffer& desc, uint32_t binding, uint32_t index) const;

        /**
         * \brief Update range of acceleration structures at [firstIndex, firstIndex + count).
         * \param desc List of acceleration structures.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setAccelerationStructureRange(std::span<const VulkanTopLevelAccelerationStructure*> desc,
                                           uint32_t                                              binding,
                                           uint32_t                                              firstIndex) const;

        /**
         * \brief Update range of sampled images at [firstIndex, firstIndex + count).
         * \param desc List of images.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setSampledImageRange(std::span<const VulkanImageView*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of sampled images at [firstIndex, firstIndex + count).
         * \param desc List of images.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setSampledImageRange(std::span<const Texture2D2*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of samplers at [firstIndex, firstIndex + count).
         * \param desc List of samplers.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setSamplerRange(std::span<const VulkanSampler*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of samplers at [firstIndex, firstIndex + count).
         * \param desc List of samplers.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setSamplerRange(std::span<const Sampler2D*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of samplers at [firstIndex, firstIndex + count).
         * \param desc List of samplers.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setSamplerRange(std::span<const Texture2D2*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of storage buffers at [firstIndex, firstIndex + count).
         * \param desc List of storage buffers.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setStorageBufferRange(std::span<const IBuffer*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of storage images at [firstIndex, firstIndex + count).
         * \param desc List of images.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setStorageImageRange(std::span<const VulkanImageView*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of storage images at [firstIndex, firstIndex + count).
         * \param desc List of images.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setStorageImageRange(std::span<const Texture2D2*> desc, uint32_t binding, uint32_t firstIndex) const;

        /**
         * \brief Update range of uniform buffers at [firstIndex, firstIndex + count).
         * \param desc List of uniform buffers.
         * \param binding Binding index.
         * \param firstIndex Index into binding, if it is an array.
         */
        void setUniformBufferRange(std::span<const IBuffer*> desc, uint32_t binding, uint32_t firstIndex) const;

    private:
        template<DescType D, typename T>
        void setRangeImpl(const std::span<const T*> desc, const uint32_t binding, const uint32_t firstIndex) const
        {
            const auto& layoutBinding = getLayoutBinding<D>(binding);

            // Verify range.
            if (firstIndex + desc.size() > layoutBinding.count)
                throw SolError(
                  std::format("Cannot update descriptor binding {}. Trying to update the range [{}, {}) but "
                              "binding only has {} elements.",
                              binding,
                              firstIndex,
                              firstIndex + desc.size(),
                              desc.size()));

            VkDescriptorGetInfoEXT info{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT, .pNext = nullptr};
            size_t                 elementSize = 0;
            if constexpr (D == DescType::AccelerationStructure)
            {
                info.type   = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                elementSize = descriptorBuffer->properties->accelerationStructureDescriptorSize;
            }
            else if constexpr (D == DescType::SampledImage)
            {
                info.type   = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                elementSize = descriptorBuffer->properties->sampledImageDescriptorSize;
            }
            else if constexpr (D == DescType::Sampler)
            {
                info.type   = VK_DESCRIPTOR_TYPE_SAMPLER;
                elementSize = descriptorBuffer->properties->samplerDescriptorSize;
            }
            else if constexpr (D == DescType::StorageBuffer)
            {
                info.type   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                elementSize = descriptorBuffer->properties->storageBufferDescriptorSize;
            }
            else if constexpr (D == DescType::StorageImage)
            {
                info.type   = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                elementSize = descriptorBuffer->properties->storageImageDescriptorSize;
            }
            else if constexpr (D == DescType::UniformBuffer)
            {
                info.type   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                elementSize = descriptorBuffer->properties->uniformBufferDescriptorSize;
            }
            else
                []<bool Flag = false>() { static_assert(Flag, "Unknown descriptor type."); }
            ();

            // Calculate start offset.
            auto bufferOffset = descriptorBuffer->getBuffer().getMappedData<void*>() + virtualOffset +
                                layoutBinding.offset + firstIndex * elementSize;

            for (const auto* d : desc)
            {
                VkDescriptorAddressInfoEXT bufferInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT};
                info.data.pStorageBuffer              = &bufferInfo;
                info.data.pUniformBuffer              = &bufferInfo;

                VkDescriptorImageInfo imageInfo = {};
                info.data.pSampledImage         = nullptr;
                info.data.pStorageImage         = nullptr;

                if constexpr (D == DescType::AccelerationStructure)
                {
                    if constexpr (std::same_as<VulkanTopLevelAccelerationStructure, T>)
                        info.data.accelerationStructure = d->getDeviceAddress();
                    else
                        []<bool Flag = false>() { static_assert(Flag, "Unknown descriptor type."); }
                    ();
                }
                else if constexpr (D == DescType::SampledImage || D == DescType::StorageImage)
                {
                    if constexpr (std::same_as<VulkanImageView, T>)
                        imageInfo.imageView = d->get();
                    else if constexpr (std::same_as<Texture2D2, T>)
                        imageInfo.imageView = d->getImageView().get();
                    else
                        []<bool Flag = false>() { static_assert(Flag, "Unknown descriptor type."); }
                    ();

                    info.data.pSampledImage = &imageInfo;
                    info.data.pStorageImage = &imageInfo;
                }
                else if constexpr (D == DescType::Sampler)
                {
                    if constexpr (std::same_as<VulkanSampler, T>)
                        info.data.pSampler = &d->get();
                    else if constexpr (std::same_as<Sampler2D, T>)
                        info.data.pSampler = &d->getSampler().get();
                    else if constexpr (std::same_as<Texture2D2, T>)
                        info.data.pSampler = &d->getSampler().getSampler().get();
                    else
                        []<bool Flag = false>() { static_assert(Flag, "Unknown descriptor type."); }
                    ();
                }
                else if constexpr (D == DescType::StorageBuffer || D == DescType::UniformBuffer)
                {
                    if constexpr (std::derived_from<IBuffer, T>)
                    {
                        bufferInfo.address = d->getBuffer().getDeviceAddress() + d->getBufferOffset();
                        bufferInfo.range   = d->getBufferSize();
                        bufferInfo.format  = VK_FORMAT_UNDEFINED;
                    }
                    else
                        []<bool Flag = false>() { static_assert(Flag, "Unknown descriptor type."); }
                    ();

                    info.data.pStorageBuffer = &bufferInfo;
                    info.data.pUniformBuffer = &bufferInfo;
                }
                else
                    []<bool Flag = false>() { static_assert(Flag, "Unknown descriptor type."); }
                ();

                // Write descriptor to buffer.
                descriptorBuffer->getDevice().vkGetDescriptorEXT(
                  descriptorBuffer->getDevice().get(), &info, elementSize, bufferOffset);

                bufferOffset += elementSize;
            }
        }

        template<DescType>
        [[nodiscard]] const auto& getLayoutBinding(const uint32_t) const
        {
            []<bool Flag = false>() { static_assert(Flag, "Unknown descriptor type."); }
            ();
            return *this;
        }

        template<DescType D>
            requires(D == DescType::AccelerationStructure)
        [[nodiscard]] const auto& getLayoutBinding(const uint32_t binding) const
        {
            return descriptorLayout->getAccelerationStructure(binding);
        }

        template<DescType D>
            requires(D == DescType::SampledImage)
        [[nodiscard]] const auto& getLayoutBinding(const uint32_t binding) const
        {
            return descriptorLayout->getSampledImage(binding);
        }

        template<DescType D>
            requires(D == DescType::Sampler)
        [[nodiscard]] const auto& getLayoutBinding(const uint32_t binding) const
        {
            return descriptorLayout->getSampler(binding);
        }

        template<DescType D>
            requires(D == DescType::StorageBuffer)
        [[nodiscard]] const auto& getLayoutBinding(const uint32_t binding) const
        {
            return descriptorLayout->getStorageBuffer(binding);
        }

        template<DescType D>
            requires(D == DescType::StorageImage)
        [[nodiscard]] const auto& getLayoutBinding(const uint32_t binding) const
        {
            return descriptorLayout->getStorageImage(binding);
        }

        template<DescType D>
            requires(D == DescType::UniformBuffer)
        [[nodiscard]] const auto& getLayoutBinding(const uint32_t binding) const
        {
            return descriptorLayout->getUniformBuffer(binding);
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        DescriptorBuffer* descriptorBuffer = nullptr;

        const DescriptorLayout* descriptorLayout = nullptr;

        VmaVirtualAllocation virtualAllocation;

        /**
         * \brief Offset of the descriptor into the descriptor buffer in bytes.
         */
        VkDeviceSize virtualOffset = 0;
    };
}  // namespace sol
