#include "sol-material/material2.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid_system_generator.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Material2::Material2(const uuids::uuid id, VulkanDevice& device, std::vector<const DescriptorLayout*> layouts) :
        uuid(id), device(&device), descriptorLayouts(std::move(layouts))
    {
    }

    Material2::Material2(VulkanDevice& device, std::vector<const DescriptorLayout*> layouts) :
        uuid(uuids::uuid_system_generator{}()), device(&device), descriptorLayouts(std::move(layouts))
    {
    }

    Material2::~Material2() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& Material2::getDevice() noexcept { return *device; }

    const VulkanDevice& Material2::getDevice() const noexcept { return *device; }

    const uuids::uuid& Material2::getUuid() const noexcept { return uuid; }

    const std::vector<const DescriptorLayout*>& Material2::getDescriptorLayouts() const noexcept
    {
        return descriptorLayouts;
    }

    size_t Material2::getPushConstantSize() const noexcept
    {
        // Calculate if not initialized yet.
        if (pushConstantSize == ~0ULL)
        {
            const auto& elem = std::ranges::max_element(
              getPushConstantRanges(), {}, [](const auto& range) { return range.offset + range.size; });
            pushConstantSize = elem->offset + elem->size;
        }

        return pushConstantSize;
    }

}  // namespace sol
