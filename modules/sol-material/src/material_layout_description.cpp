#include "sol-material/material_layout_description.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <ranges>

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Subtypes.
    ////////////////////////////////////////////////////////////////

    bool MaterialLayoutDescription::AccelerationStructureBinding::operator==(
      const AccelerationStructureBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    bool MaterialLayoutDescription::CombinedImageSamplerBinding::operator==(
      const CombinedImageSamplerBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && count == rhs.count && stages == rhs.stages;
    }

    bool MaterialLayoutDescription::StorageBufferBinding::operator==(const StorageBufferBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    bool MaterialLayoutDescription::StorageImageBinding::operator==(const StorageImageBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    bool MaterialLayoutDescription::UniformBufferBinding::operator==(const UniformBufferBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && size == rhs.size && count == rhs.count &&
               stages == rhs.stages;
    }

    bool MaterialLayoutDescription::PushConstant::operator==(const PushConstant& rhs) const noexcept
    {
        return offset == rhs.offset && size == rhs.size && stages == rhs.stages;
    }
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MaterialLayoutDescription::MaterialLayoutDescription() = default;

    MaterialLayoutDescription::MaterialLayoutDescription(const MaterialLayoutDescription&) = default;

    MaterialLayoutDescription::MaterialLayoutDescription(MaterialLayoutDescription&&) noexcept = default;

    MaterialLayoutDescription::~MaterialLayoutDescription() noexcept = default;

    MaterialLayoutDescription& MaterialLayoutDescription::operator=(const MaterialLayoutDescription&) = default;

    MaterialLayoutDescription& MaterialLayoutDescription::operator=(MaterialLayoutDescription&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    size_t MaterialLayoutDescription::getSetCount() const noexcept
    {
        const auto set = [](const auto& binding) -> uint32_t { return binding.set; };

        uint32_t max = 0;
        if (!accelerationStructures.empty())
            max = std::max(max, 1 + std::ranges::max(accelerationStructures | std::views::transform(set)));
        if (!combinedImageSamplers.empty())
            max = std::max(max, 1 + std::ranges::max(combinedImageSamplers | std::views::transform(set)));
        if (!storageBuffers.empty())
            max = std::max(max, 1 + std::ranges::max(storageBuffers | std::views::transform(set)));
        if (!storageImages.empty())
            max = std::max(max, 1 + std::ranges::max(storageImages | std::views::transform(set)));
        if (!uniformBuffers.empty())
            max = std::max(max, 1 + std::ranges::max(uniformBuffers | std::views::transform(set)));

        return max;
    }

    size_t MaterialLayoutDescription::getBindingCount() const noexcept
    {
        return getAccelerationStructureCount() + getCombinedImageSamplerCount() + getStorageBufferCount() +
               getStorageImageCount() + getUniformBufferCount();
    }

    size_t MaterialLayoutDescription::getAccelerationStructureCount() const noexcept
    {
        return accelerationStructures.size();
    }

    size_t MaterialLayoutDescription::getAccelerationStructureCount(const size_t set) const noexcept
    {
        return std::ranges::count_if(accelerationStructures, [set](const auto& b) { return b.set == set; });
    }

    size_t MaterialLayoutDescription::getCombinedImageSamplerCount() const noexcept
    {
        return combinedImageSamplers.size();
    }

    size_t MaterialLayoutDescription::getCombinedImageSamplerCount(const size_t set) const noexcept
    {
        return std::ranges::count_if(combinedImageSamplers, [set](const auto& b) { return b.set == set; });
    }

    size_t MaterialLayoutDescription::getStorageBufferCount() const noexcept { return storageBuffers.size(); }

    size_t MaterialLayoutDescription::getStorageBufferCount(const size_t set) const noexcept
    {
        return std::ranges::count_if(storageBuffers, [set](const auto& b) { return b.set == set; });
    }

    size_t MaterialLayoutDescription::getStorageImageCount() const noexcept { return storageImages.size(); }

    size_t MaterialLayoutDescription::getStorageImageCount(const size_t set) const noexcept
    {
        return std::ranges::count_if(storageImages, [set](const auto& b) { return b.set == set; });
    }

    size_t MaterialLayoutDescription::getUniformBufferCount() const noexcept { return uniformBuffers.size(); }

    size_t MaterialLayoutDescription::getUniformBufferCount(const size_t set) const noexcept
    {
        return std::ranges::count_if(uniformBuffers, [set](const auto& b) { return b.set == set; });
    }

    ////////////////////////////////////////////////////////////////
    // Compatibility.
    ////////////////////////////////////////////////////////////////

    uint32_t MaterialLayoutDescription::getDescriptorSetCompatibility(const MaterialLayoutDescription& other) const
    {
        auto set = std::numeric_limits<uint32_t>::max();

        // Gets set index of first incompatible binding.
        const auto func = [](const auto& b0, const auto& b1) {
            if (const auto it = std::ranges::mismatch(b0, b1, {}).in1; it != b0.end()) return it->set;
            return std::numeric_limits<uint32_t>::max();
        };

        set = std::min(set, func(accelerationStructures, other.accelerationStructures));
        set = std::min(set, func(combinedImageSamplers, other.combinedImageSamplers));
        set = std::min(set, func(storageBuffers, other.storageBuffers));
        set = std::min(set, func(storageImages, other.storageImages));
        set = std::min(set, func(uniformBuffers, other.uniformBuffers));
        return 0;
    }

    bool MaterialLayoutDescription::isPushConstantCompatible(const MaterialLayoutDescription& other) const
    {
        return std::ranges::equal(pushConstants, other.pushConstants);
    }

}  // namespace sol
