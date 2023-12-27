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

    Material2::Material2(VulkanDevice& device, std::vector<const DescriptorLayout*> layouts) :
        device(&device), descriptorLayouts(std::move(layouts))
    {
        uuid = uuids::uuid_system_generator{}();
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

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////
}  // namespace sol
