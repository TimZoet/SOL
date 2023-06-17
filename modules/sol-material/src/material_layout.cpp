#include "sol-material/material_layout.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-error/sol_error.h"

namespace
{
    [[nodiscard]] auto getSetBindings(const auto& bindings, const uint32_t set)
    {
        size_t start = bindings.size(), end = bindings.size();
        for (size_t i = 0; i < bindings.size(); ++i)
        {
            if (bindings[i].set == set)
            {
                start = i;
                break;
            }
        }

        for (size_t i = start; i < bindings.size(); ++i)
        {
            if (bindings[i].set == set + 1)
            {
                end = i;
                break;
            }
        }

        return std::span{bindings.data() + start, end - start};
    }
}  // namespace

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MaterialLayout::MaterialLayout(VulkanDevice& device) : device(&device) {}

    MaterialLayout::~MaterialLayout() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool MaterialLayout::isFinalized() const noexcept { return finalized; }

    const MaterialLayoutDescription& MaterialLayout::getDescription() const
    {
        requireFinalized();
        return description;
    }

    size_t MaterialLayout::getSetCount() const noexcept
    {
        requireFinalized();
        return layouts.size();
    }

    std::span<const MaterialLayoutDescription::AccelerationStructureBinding>
      MaterialLayout::getAccelerationStructures(const uint32_t set) const
    {
        requireFinalized();
        return getSetBindings(description.accelerationStructures, set);
    }

    std::span<const MaterialLayoutDescription::CombinedImageSamplerBinding>
      MaterialLayout::getCombinedImageSamplers(const uint32_t set) const
    {
        requireFinalized();
        return getSetBindings(description.combinedImageSamplers, set);
    }

    std::span<const MaterialLayoutDescription::StorageBufferBinding>
      MaterialLayout::getStorageBuffers(const uint32_t set) const
    {
        requireFinalized();
        return getSetBindings(description.storageBuffers, set);
    }

    std::span<const MaterialLayoutDescription::StorageImageBinding>
      MaterialLayout::getStorageImages(const uint32_t set) const
    {
        requireFinalized();
        return getSetBindings(description.storageImages, set);
    }

    std::span<const MaterialLayoutDescription::UniformBufferBinding>
      MaterialLayout::getUniformBuffers(const uint32_t set) const
    {
        requireFinalized();
        return getSetBindings(description.uniformBuffers, set);
    }

    size_t MaterialLayout::getPushConstantCount() const noexcept
    {
        requireFinalized();
        return description.pushConstants.size();
    }

    size_t MaterialLayout::getPushConstantTotalSize() const noexcept
    {
        requireFinalized();
        if (description.pushConstants.empty()) return 0;
        return description.pushConstants.back().offset + description.pushConstants.back().size;
    }

    const std::vector<VulkanDescriptorSetLayoutPtr>& MaterialLayout::getDescriptorSetLayouts() const
    {
        requireFinalized();
        return layouts;
    }

    const VulkanDescriptorSetLayout& MaterialLayout::getDescriptorSetLayout(const size_t set) const
    {
        requireFinalized();
        return *layouts.at(set);
    }

    const std::vector<VkPushConstantRange>& MaterialLayout::getPushConstants() const
    {
        requireFinalized();
        return pushConstants;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    void MaterialLayout::finalize(MaterialLayoutDescription desc)
    {
        requireNonFinalized();

        if (!device) throw SolError("Cannot finalize MaterialLayout. It does not have a device assigned.");

        // Sort bindings by set and binding index.
        const auto sorter = [](const auto& lhs, const auto& rhs) {
            return lhs.set < rhs.set || lhs.set == rhs.set && lhs.binding < rhs.binding;
        };
        std::ranges::sort(desc.accelerationStructures.begin(), desc.accelerationStructures.end(), sorter);
        std::ranges::sort(desc.combinedImageSamplers.begin(), desc.combinedImageSamplers.end(), sorter);
        std::ranges::sort(desc.storageBuffers.begin(), desc.storageBuffers.end(), sorter);
        std::ranges::sort(desc.storageImages.begin(), desc.storageImages.end(), sorter);
        std::ranges::sort(desc.uniformBuffers.begin(), desc.uniformBuffers.end(), sorter);
        std::ranges::sort(desc.pushConstants.begin(), desc.pushConstants.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.offset < rhs.offset;
        });

        // Create layouts.
        std::vector<VulkanDescriptorSetLayout::Settings> layoutSettings(desc.getSetCount(), {.device = *this->device});
        layouts.reserve(layoutSettings.size());
        for (const auto& [name, set, binding, stages] : desc.accelerationStructures)
        {
            layoutSettings[set].bindings.emplace_back(
              binding, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1, stages, nullptr);
        }
        for (const auto& [name, set, binding, count, stages] : desc.combinedImageSamplers)
        {
            layoutSettings[set].bindings.emplace_back(
              binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count, stages, nullptr);
        }
        for (const auto& [name, set, binding, stages] : desc.storageBuffers)
        {
            layoutSettings[set].bindings.emplace_back(binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, stages, nullptr);
        }
        for (const auto& [name, set, binding, stages] : desc.storageImages)
        {
            layoutSettings[set].bindings.emplace_back(binding, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, stages, nullptr);
        }
        for (const auto& [name, set, binding, size, count, stages, usage, sharing] : desc.uniformBuffers)
        {
            layoutSettings[set].bindings.emplace_back(
              binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, count, stages, nullptr);
        }
        for (const auto& settings : layoutSettings) layouts.emplace_back(VulkanDescriptorSetLayout::create(settings));

        // Create push constants.
        pushConstants.reserve(desc.pushConstants.size());
        for (const auto& pc : desc.pushConstants) pushConstants.emplace_back(pc.stages, pc.offset, pc.size);

        description = std::move(desc);
        finalized   = true;
    }

    void MaterialLayout::requireFinalized() const
    {
        if (!finalized) throw SolError("MaterialLayout was not yet finalized.");
    }

    void MaterialLayout::requireNonFinalized() const
    {
        if (finalized) throw SolError("MaterialLayout was already finalized.");
    }
}  // namespace sol
