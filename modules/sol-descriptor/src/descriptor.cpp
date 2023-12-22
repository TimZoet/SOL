#include "sol-descriptor/descriptor.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Descriptor::Descriptor(DescriptorBuffer&          buffer,
                           const DescriptorLayout&    layout,
                           const VmaVirtualAllocation allocation,
                           const VkDeviceSize         offset) :
        descriptorBuffer(&buffer), descriptorLayout(&layout), virtualAllocation(allocation), virtualOffset(offset)
    {
    }

    Descriptor::~Descriptor() noexcept { vmaVirtualFree(descriptorBuffer->getVirtualBlock(), virtualAllocation); }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    DescriptorBuffer& Descriptor::getBuffer() { return *descriptorBuffer; }

    const DescriptorBuffer& Descriptor::getBuffer() const { return *descriptorBuffer; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Descriptor::setAccelerationStructure(const VulkanTopLevelAccelerationStructure& desc,
                                              const uint32_t                             binding,
                                              const uint32_t                             index) const
    {
        const auto* d = &desc;
        setAccelerationStructureRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setSampledImage(const VulkanImageView& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setSampledImageRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setSampledImage(const Texture2D2& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setSampledImageRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setSampler(const VulkanSampler& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setSamplerRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setSampler(const Sampler2D& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setSamplerRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setSampler(const Texture2D2& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setSamplerRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setStorageBuffer(const IBuffer& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setStorageBufferRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setStorageImage(const VulkanImageView& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setStorageImageRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setStorageImage(const Texture2D2& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setStorageImageRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setUniformBuffer(const IBuffer& desc, const uint32_t binding, const uint32_t index) const
    {
        const auto* d = &desc;
        setUniformBufferRange(std::span(&d, 1), binding, index);
    }

    void Descriptor::setAccelerationStructureRange(const std::span<const VulkanTopLevelAccelerationStructure*> desc,
                                                   const uint32_t                                              binding,
                                                   const uint32_t firstIndex) const
    {
        setRangeImpl<DescType::AccelerationStructure>(desc, binding, firstIndex);
    }

    void Descriptor::setSampledImageRange(const std::span<const VulkanImageView*> desc,
                                          const uint32_t                          binding,
                                          const uint32_t                          firstIndex) const
    {
        setRangeImpl<DescType::SampledImage>(desc, binding, firstIndex);
    }

    void Descriptor::setSampledImageRange(const std::span<const Texture2D2*> desc,
                                          const uint32_t                     binding,
                                          const uint32_t                     firstIndex) const
    {
        setRangeImpl<DescType::SampledImage>(desc, binding, firstIndex);
    }

    void Descriptor::setSamplerRange(const std::span<const VulkanSampler*> desc,
                                     const uint32_t                        binding,
                                     const uint32_t                        firstIndex) const
    {
        setRangeImpl<DescType::Sampler>(desc, binding, firstIndex);
    }

    void Descriptor::setSamplerRange(const std::span<const Sampler2D*> desc,
                                     const uint32_t                    binding,
                                     const uint32_t                    firstIndex) const
    {
        setRangeImpl<DescType::Sampler>(desc, binding, firstIndex);
    }

    void Descriptor::setSamplerRange(const std::span<const Texture2D2*> desc,
                                     const uint32_t                     binding,
                                     const uint32_t                     firstIndex) const
    {
        setRangeImpl<DescType::Sampler>(desc, binding, firstIndex);
    }

    void Descriptor::setStorageBufferRange(const std::span<const IBuffer*> desc,
                                           const uint32_t                  binding,
                                           const uint32_t                  firstIndex) const
    {
        setRangeImpl<DescType::StorageBuffer>(desc, binding, firstIndex);
    }

    void Descriptor::setStorageImageRange(const std::span<const VulkanImageView*> desc,
                                          const uint32_t                          binding,
                                          const uint32_t                          firstIndex) const
    {
        setRangeImpl<DescType::StorageImage>(desc, binding, firstIndex);
    }

    void Descriptor::setStorageImageRange(const std::span<const Texture2D2*> desc,
                                          const uint32_t                     binding,
                                          const uint32_t                     firstIndex) const
    {
        setRangeImpl<DescType::StorageImage>(desc, binding, firstIndex);
    }

    void Descriptor::setUniformBufferRange(const std::span<const IBuffer*> desc,
                                           const uint32_t                  binding,
                                           const uint32_t                  firstIndex) const
    {
        setRangeImpl<DescType::UniformBuffer>(desc, binding, firstIndex);
    }
}  // namespace sol
