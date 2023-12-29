#include "sol-material/material2.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Material2::Material2(VulkanDevice& device, const uuids::uuid id, std::vector<const DescriptorLayout*> layouts) :
        device(&device), uuid(id), descriptorLayouts(std::move(layouts))
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
}  // namespace sol
