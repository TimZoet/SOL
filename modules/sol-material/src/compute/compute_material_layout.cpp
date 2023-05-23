#include "sol-material/compute/compute_material_layout.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Subtypes.
    ////////////////////////////////////////////////////////////////

    bool ComputeMaterialLayout::StorageImageBinding::operator==(const StorageImageBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    bool ComputeMaterialLayout::StorageBufferBinding::operator==(const StorageBufferBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialLayout::ComputeMaterialLayout(VulkanDevice& vkDevice) : device(&vkDevice) {}

    ComputeMaterialLayout::~ComputeMaterialLayout() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool ComputeMaterialLayout::isFinalized() const noexcept { return finalized; }

    const std::vector<ComputeMaterialLayout::StorageImageBindingPtr>&
      ComputeMaterialLayout::getStorageImages() const noexcept
    {
        return storageImages;
    }

    std::pair<const ComputeMaterialLayout::StorageImageBindingPtr*, size_t>
      ComputeMaterialLayout::getStorageImages(const uint32_t set) const
    {
        requireFinalized();

        size_t start = storageImages.size(), end = storageImages.size();
        for (size_t i = 0; i < storageImages.size(); i++)
        {
            if (storageImages[i]->set == set)
            {
                start = i;
                break;
            }
        }

        for (size_t i = start; i < storageImages.size(); i++)
        {
            if (storageImages[i]->set == set + 1)
            {
                end = i;
                break;
            }
        }

        return std::make_pair(storageImages.data() + start, end - start);
    }

    const std::vector<ComputeMaterialLayout::StorageBufferBindingPtr>&
      ComputeMaterialLayout::getStorageBuffers() const noexcept
    {
        return storageBuffers;
    }

    std::pair<const ComputeMaterialLayout::StorageBufferBindingPtr*, size_t>
      ComputeMaterialLayout::getStorageBuffers(const uint32_t set) const
    {
        requireFinalized();

        size_t start = storageBuffers.size(), end = storageBuffers.size();
        for (size_t i = 0; i < storageBuffers.size(); i++)
        {
            if (storageBuffers[i]->set == set)
            {
                start = i;
                break;
            }
        }

        for (size_t i = start; i < storageBuffers.size(); i++)
        {
            if (storageBuffers[i]->set == set + 1)
            {
                end = i;
                break;
            }
        }

        return std::make_pair(storageBuffers.data() + start, end - start);
    }

    size_t ComputeMaterialLayout::getSetCount() const noexcept
    {
        if (isFinalized()) return finalLayouts.size();

        // Calculate highest set index of storage buffers.
        uint32_t max = 0;
        for (const auto& b : storageBuffers) max = std::max(max, b->set);

        return max + static_cast<size_t>(1);
    }

    size_t ComputeMaterialLayout::getStorageImageCount() const noexcept { return storageImages.size(); }

    size_t ComputeMaterialLayout::getStorageBufferCount() const noexcept { return storageBuffers.size(); }

    const std::vector<VulkanDescriptorSetLayoutPtr>& ComputeMaterialLayout::getFinalizedDescriptorSetLayouts() const
    {
        requireFinalized();
        return finalLayouts;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialLayout::StorageImageBinding& ComputeMaterialLayout::addStorageImage()
    {
        requireNonFinalized();
        return *storageImages.emplace_back(std::make_unique<StorageImageBinding>());
    }

    ComputeMaterialLayout::StorageBufferBinding& ComputeMaterialLayout::addStorageBuffer()
    {
        requireNonFinalized();
        return *storageBuffers.emplace_back(std::make_unique<StorageBufferBinding>());
    }

    void ComputeMaterialLayout::finalize()
    {
        requireNonFinalized();

        // Sort everything by set and binding index.
        std::ranges::sort(storageImages.begin(), storageImages.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->set < rhs->set || lhs->set == rhs->set && lhs->binding < rhs->binding;
        });
        std::ranges::sort(storageBuffers.begin(), storageBuffers.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->set < rhs->set || lhs->set == rhs->set && lhs->binding < rhs->binding;
        });

        // Prepare settings for each set.
        std::vector<VulkanDescriptorSetLayout::Settings> layoutSettings(getSetCount(), {.device = *this->device});

        // Add storage images.
        for (const auto& ub : storageImages)
        {
            layoutSettings[ub->set].bindings.emplace_back(
              ub->binding, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, ub->stages, nullptr);
        }

        // Add storage buffers.
        for (const auto& ub : storageBuffers)
        {
            layoutSettings[ub->set].bindings.emplace_back(
              ub->binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, ub->stages, nullptr);
        }

        // Create layouts.
        finalLayouts.reserve(layoutSettings.size());
        for (const auto& settings : layoutSettings)
            finalLayouts.emplace_back(VulkanDescriptorSetLayout::create(settings));

        finalized = true;
    }

    void ComputeMaterialLayout::requireFinalized() const
    {
        if (!finalized) throw SolError("ComputeMaterialLayout was not yet finalized.");
    }

    void ComputeMaterialLayout::requireNonFinalized() const
    {
        if (finalized) throw SolError("ComputeMaterialLayout was already finalized.");
    }

    ////////////////////////////////////////////////////////////////
    // Compatibility.
    ////////////////////////////////////////////////////////////////

    uint32_t ComputeMaterialLayout::getDescriptorSetCompatibility(const ComputeMaterialLayout& other) const
    {
        requireFinalized();

        auto set = static_cast<uint32_t>(finalLayouts.size());

        // Get set index of first incompatible storage buffer.
        if (const auto it = std::ranges::mismatch(
                              storageBuffers,
                              other.storageBuffers,
                              {},
                              [](const auto& ptr) { return *ptr; },
                              [](const auto& ptr) { return *ptr; })
                              .in1;
            it != storageBuffers.end())
            set = std::min(set, (*it)->set);

        return set;
    }
}  // namespace sol
