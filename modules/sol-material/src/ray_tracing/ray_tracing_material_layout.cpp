#include "sol-material/ray_tracing/ray_tracing_material_layout.h"

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

    bool RayTracingMaterialLayout::StorageImageBinding::operator==(const StorageImageBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    bool RayTracingMaterialLayout::StorageBufferBinding::operator==(const StorageBufferBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    bool RayTracingMaterialLayout::AccelerationStructureBinding::operator==(
      const AccelerationStructureBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && stages == rhs.stages;
    }

    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterialLayout::RayTracingMaterialLayout(VulkanDevice& vkDevice) : device(&vkDevice) {}

    RayTracingMaterialLayout::~RayTracingMaterialLayout() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool RayTracingMaterialLayout::isFinalized() const noexcept { return finalized; }

    const std::vector<RayTracingMaterialLayout::StorageImageBindingPtr>&
      RayTracingMaterialLayout::getStorageImages() const noexcept
    {
        return storageImages;
    }

    std::pair<const RayTracingMaterialLayout::StorageImageBindingPtr*, size_t>
      RayTracingMaterialLayout::getStorageImages(const uint32_t set) const
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

    const std::vector<RayTracingMaterialLayout::StorageBufferBindingPtr>&
      RayTracingMaterialLayout::getStorageBuffers() const noexcept
    {
        return storageBuffers;
    }

    std::pair<const RayTracingMaterialLayout::StorageBufferBindingPtr*, size_t>
      RayTracingMaterialLayout::getStorageBuffers(const uint32_t set) const
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

    const std::vector<RayTracingMaterialLayout::AccelerationStructureBindingPtr>&
      RayTracingMaterialLayout::getAccelerationStructures() const noexcept
    {
        return accelerationStructures;
    }

    std::pair<const RayTracingMaterialLayout::AccelerationStructureBindingPtr*, size_t>
      RayTracingMaterialLayout::getAccelerationStructures(const uint32_t set) const
    {
        requireFinalized();

        size_t start = accelerationStructures.size(), end = accelerationStructures.size();
        for (size_t i = 0; i < accelerationStructures.size(); i++)
        {
            if (accelerationStructures[i]->set == set)
            {
                start = i;
                break;
            }
        }

        for (size_t i = start; i < accelerationStructures.size(); i++)
        {
            if (accelerationStructures[i]->set == set + 1)
            {
                end = i;
                break;
            }
        }

        return std::make_pair(accelerationStructures.data() + start, end - start);
    }

    size_t RayTracingMaterialLayout::getSetCount() const noexcept
    {
        if (isFinalized()) return finalLayouts.size();

        // Calculate highest set index of storage buffers.
        uint32_t max = 0;
        for (const auto& b : storageBuffers) max = std::max(max, b->set);

        return max + static_cast<size_t>(1);
    }

    size_t RayTracingMaterialLayout::getStorageImageCount() const noexcept { return storageImages.size(); }

    size_t RayTracingMaterialLayout::getStorageBufferCount() const noexcept { return storageBuffers.size(); }

    size_t RayTracingMaterialLayout::getAccelerationStructureCount() const noexcept
    {
        return accelerationStructures.size();
    }

    const std::vector<VulkanDescriptorSetLayoutPtr>& RayTracingMaterialLayout::getFinalizedDescriptorSetLayouts() const
    {
        requireFinalized();
        return finalLayouts;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterialLayout::StorageImageBinding& RayTracingMaterialLayout::addStorageImage()
    {
        requireNonFinalized();
        return *storageImages.emplace_back(std::make_unique<StorageImageBinding>());
    }

    RayTracingMaterialLayout::StorageBufferBinding& RayTracingMaterialLayout::addStorageBuffer()
    {
        requireNonFinalized();
        return *storageBuffers.emplace_back(std::make_unique<StorageBufferBinding>());
    }

    RayTracingMaterialLayout::AccelerationStructureBinding& RayTracingMaterialLayout::addAccelerationStructure()
    {
        requireNonFinalized();
        return *accelerationStructures.emplace_back(std::make_unique<AccelerationStructureBinding>());
    }

    void RayTracingMaterialLayout::finalize()
    {
        requireNonFinalized();

        // Sort everything by set and binding index.
        std::ranges::sort(storageImages.begin(), storageImages.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->set < rhs->set || lhs->set == rhs->set && lhs->binding < rhs->binding;
        });
        std::ranges::sort(storageBuffers.begin(), storageBuffers.end(), [](const auto& lhs, const auto& rhs) {
            return lhs->set < rhs->set || lhs->set == rhs->set && lhs->binding < rhs->binding;
        });
        std::ranges::sort(
          accelerationStructures.begin(), accelerationStructures.end(), [](const auto& lhs, const auto& rhs) {
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

        // Add acceleration structures.
        for (const auto& ub : accelerationStructures)
        {
            layoutSettings[ub->set].bindings.emplace_back(
              ub->binding, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1, ub->stages, nullptr);
        }

        // Create layouts.
        finalLayouts.reserve(layoutSettings.size());
        for (const auto& settings : layoutSettings)
            finalLayouts.emplace_back(VulkanDescriptorSetLayout::create(settings));

        finalized = true;
    }

    void RayTracingMaterialLayout::requireFinalized() const
    {
        if (!finalized) throw SolError("RayTracingMaterialLayout was not yet finalized.");
    }

    void RayTracingMaterialLayout::requireNonFinalized() const
    {
        if (finalized) throw SolError("RayTracingMaterialLayout was already finalized.");
    }

    ////////////////////////////////////////////////////////////////
    // Compatibility.
    ////////////////////////////////////////////////////////////////

    uint32_t RayTracingMaterialLayout::getDescriptorSetCompatibility(const RayTracingMaterialLayout& other) const
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
