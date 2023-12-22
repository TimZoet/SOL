#include "sol-descriptor/descriptor_layout.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    DescriptorLayout::DescriptorLayout(VulkanDevice& d) : device(&d) {}

    DescriptorLayout::~DescriptorLayout() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    size_t DescriptorLayout::getLayoutSize() const
    {
        requireFinalized();
        return layoutSize;
    }

    ////////////////////////////////////////////////////////////////
    // Add.
    ////////////////////////////////////////////////////////////////

    const std::vector<DescriptorLayout::AccelerationStructureBinding>&
      DescriptorLayout::getAccelerationStructures() const noexcept
    {
        return accelerationStructures;
    }

    const std::vector<DescriptorLayout::SampledImageBinding>& DescriptorLayout::getSampledImages() const noexcept
    {
        return sampledImages;
    }

    const std::vector<DescriptorLayout::SamplerBinding>& DescriptorLayout::getSamplers() const noexcept
    {
        return samplers;
    }

    const std::vector<DescriptorLayout::StorageBufferBinding>& DescriptorLayout::getStorageBuffers() const noexcept
    {
        return storageBuffers;
    }

    const std::vector<DescriptorLayout::StorageImageBinding>& DescriptorLayout::getStorageImages() const noexcept
    {
        return storageImages;
    }

    const std::vector<DescriptorLayout::UniformBufferBinding>& DescriptorLayout::getUniformBuffers() const noexcept
    {
        return uniformBuffers;
    }

    const DescriptorLayout::AccelerationStructureBinding&
      DescriptorLayout::getAccelerationStructure(const uint32_t binding) const
    {
        for (const auto& b : accelerationStructures)
            if (b.binding == binding) return b;

        throw SolError(
          std::format("Cannot get binding. There exists no AccelerationStructureBinding with index {}.", binding));
    }

    const DescriptorLayout::SampledImageBinding& DescriptorLayout::getSampledImage(const uint32_t binding) const
    {
        for (const auto& b : sampledImages)
            if (b.binding == binding) return b;

        throw SolError(std::format("Cannot get binding. There exists no SampledImageBinding with index {}.", binding));
    }

    const DescriptorLayout::SamplerBinding& DescriptorLayout::getSampler(const uint32_t binding) const
    {
        for (const auto& b : samplers)
            if (b.binding == binding) return b;

        throw SolError(std::format("Cannot get binding. There exists no SamplerBinding with index {}.", binding));
    }

    const DescriptorLayout::StorageBufferBinding& DescriptorLayout::getStorageBuffer(const uint32_t binding) const
    {
        for (const auto& b : storageBuffers)
            if (b.binding == binding) return b;

        throw SolError(std::format("Cannot get binding. There exists no StorageBufferBinding with index {}.", binding));
    }

    const DescriptorLayout::StorageImageBinding& DescriptorLayout::getStorageImage(const uint32_t binding) const
    {
        for (const auto& b : storageImages)
            if (b.binding == binding) return b;

        throw SolError(std::format("Cannot get binding. There exists no StorageImageBinding with index {}.", binding));
    }

    const DescriptorLayout::UniformBufferBinding& DescriptorLayout::getUniformBuffer(const uint32_t binding) const
    {
        for (const auto& b : uniformBuffers)
            if (b.binding == binding) return b;

        throw SolError(std::format("Cannot get binding. There exists no UniformBufferBinding with index {}.", binding));
    }

    void DescriptorLayout::add(AccelerationStructureBinding binding)
    {
        requireNonFinalized();
        accelerationStructures.emplace_back(binding);
    }

    void DescriptorLayout::add(SampledImageBinding binding)
    {
        requireNonFinalized();
        sampledImages.emplace_back(binding);
    }

    void DescriptorLayout::add(SamplerBinding binding)
    {
        requireNonFinalized();
        samplers.emplace_back(binding);
    }

    void DescriptorLayout::add(StorageBufferBinding binding)
    {
        requireNonFinalized();
        storageBuffers.emplace_back(binding);
    }

    void DescriptorLayout::add(StorageImageBinding binding)
    {
        requireNonFinalized();
        storageImages.emplace_back(binding);
    }

    void DescriptorLayout::add(UniformBufferBinding binding)
    {
        requireNonFinalized();
        uniformBuffers.emplace_back(binding);
    }

    ////////////////////////////////////////////////////////////////
    // Finalize.
    ////////////////////////////////////////////////////////////////

    void DescriptorLayout::finalize()
    {
        requireNonFinalized();

        // Sort bindings by index.
        const auto sorter = [](const auto& lhs, const auto& rhs) { return lhs.binding < rhs.binding; };
        std::ranges::sort(accelerationStructures.begin(), accelerationStructures.end(), sorter);
        std::ranges::sort(sampledImages.begin(), sampledImages.end(), sorter);
        std::ranges::sort(samplers.begin(), samplers.end(), sorter);
        std::ranges::sort(storageBuffers.begin(), storageBuffers.end(), sorter);
        std::ranges::sort(storageImages.begin(), storageImages.end(), sorter);
        std::ranges::sort(uniformBuffers.begin(), uniformBuffers.end(), sorter);

        // TODO: Verify that bindings do not overlap, have gaps, etc?

        VulkanDescriptorSetLayout::Settings settings;
        settings.device = *device;
        settings.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;

        // Collect bindings.

        for (const auto& b : accelerationStructures)
            settings.bindings.emplace_back(
              b.binding, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, b.count, b.stages, nullptr);

        for (const auto& b : sampledImages)
            settings.bindings.emplace_back(b.binding, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, b.count, b.stages, nullptr);

        for (const auto& b : samplers)
            settings.bindings.emplace_back(b.binding, VK_DESCRIPTOR_TYPE_SAMPLER, b.count, b.stages, nullptr);

        for (const auto& b : storageBuffers)
            settings.bindings.emplace_back(b.binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, b.count, b.stages, nullptr);

        for (const auto& b : storageImages)
            settings.bindings.emplace_back(b.binding, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, b.count, b.stages, nullptr);

        for (const auto& b : uniformBuffers)
            settings.bindings.emplace_back(b.binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, b.count, b.stages, nullptr);

        // Create layout.
        layout = VulkanDescriptorSetLayout::create(settings);

        // Retrieve size of layout and align.
        device->vkGetDescriptorSetLayoutSizeEXT(device->get(), layout->get(), &layoutSize);
        const auto& props = device->getPhysicalDevice()
                              .getProperties<VkPhysicalDeviceDescriptorBufferPropertiesEXT,
                                             VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT>();
        layoutSize =
          layoutSize + props.descriptorBufferOffsetAlignment - 1 & ~(props.descriptorBufferOffsetAlignment - 1);

        // Retrieve offsets of each binding.
        const auto getOffsets = [&](auto& vec) {
            for (auto& b : vec)
                device->vkGetDescriptorSetLayoutBindingOffsetEXT(device->get(), layout->get(), b.binding, &b.offset);
        };
        getOffsets(accelerationStructures);
        getOffsets(sampledImages);
        getOffsets(samplers);
        getOffsets(storageBuffers);
        getOffsets(storageImages);
        getOffsets(uniformBuffers);
    }

    void DescriptorLayout::requireFinalized() const
    {
        if (!layout) throw SolError("DescriptorLayout was not yet finalized.");
    }

    void DescriptorLayout::requireNonFinalized() const
    {
        if (layout) throw SolError("DescriptorLayout was already finalized.");
    }
}  // namespace sol
