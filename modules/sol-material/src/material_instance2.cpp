#include "sol-material/material_instance2.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid_system_generator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/descriptor.h"
#include "sol-descriptor/descriptor_buffer.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/material2.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MaterialInstance2::MaterialInstance2(const uuids::uuid id, Material2& mtl) : uuid(id), material(&mtl)
    {
        descriptors.resize(material->getDescriptorLayouts().size());
    }

    MaterialInstance2::MaterialInstance2(Material2& mtl) : uuid(uuids::uuid_system_generator{}()), material(&mtl)
    {
        descriptors.resize(material->getDescriptorLayouts().size());
    }

    MaterialInstance2::~MaterialInstance2() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& MaterialInstance2::getDevice() noexcept { return getMaterial().getDevice(); }

    const VulkanDevice& MaterialInstance2::getDevice() const noexcept { return getMaterial().getDevice(); }

    const uuids::uuid& MaterialInstance2::getUuid() const noexcept { return uuid; }

    const std::string& MaterialInstance2::getName() const noexcept { return name; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MaterialInstance2::setName(std::string value) { name = std::move(value); }

    Material2& MaterialInstance2::getMaterial() noexcept { return *material; }

    const Material2& MaterialInstance2::getMaterial() const noexcept { return *material; }

    ////////////////////////////////////////////////////////////////
    // Descriptors.
    ////////////////////////////////////////////////////////////////

    Descriptor& MaterialInstance2::enableDescriptor(const size_t index, DescriptorBuffer& buffer)
    {
        if (index >= descriptors.size())
            throw SolError(
              std::format("Cannot enable descriptor. Layout index {} out of range [0, {})", index, descriptors.size()));

        // Already enabled.
        if (descriptors[index]) return *descriptors[index];

        // Create new descriptor.
        const auto& layouts = material->getDescriptorLayouts();
        descriptors[index]  = buffer.allocateDescriptor(*layouts[index]);

        return *descriptors[index];
    }

    Descriptor& MaterialInstance2::enableDescriptor(const DescriptorLayout& layout, DescriptorBuffer& buffer)
    {
        const auto& layouts = material->getDescriptorLayouts();
        const auto  it      = std::ranges::find(layouts, &layout);
        if (it >= layouts.end())
            throw SolError("Cannot enable descriptor. Descriptor layout is not a part of this material.");

        return enableDescriptor(std::distance(layouts.begin(), it), buffer);
    }

    void MaterialInstance2::disableDescriptor(const size_t index)
    {
        if (index >= descriptors.size())
            throw SolError(std::format(
              "Cannot disable descriptor. Layout index {} out of range [0, {})", index, descriptors.size()));

        descriptors[index].reset();
    }

    void MaterialInstance2::disableDescriptor(const DescriptorLayout& layout)
    {
        const auto& layouts = material->getDescriptorLayouts();
        const auto  it      = std::ranges::find(layouts, &layout);
        if (it >= layouts.end())
            throw SolError("Cannot disable descriptor. Descriptor layout is not a part of this material.");

        disableDescriptor(std::distance(layouts.begin(), it));
    }

    Descriptor& MaterialInstance2::operator[](const size_t index) const
    {
        if (index >= descriptors.size())
            throw SolError(
              std::format("Cannot get descriptor. Layout index {} out of range [0, {})", index, descriptors.size()));
        if (!descriptors[index])
            throw SolError(std::format("Cannot get descriptor at index {} because it was not enabled.", index));
        return *descriptors[index];
    }

    Descriptor& MaterialInstance2::operator[](const DescriptorLayout& layout) const
    {
        const auto& layouts = material->getDescriptorLayouts();
        const auto  it      = std::ranges::find(layouts, &layout);
        if (it >= layouts.end())
            throw SolError("Cannot get descriptor. Descriptor layout is not a part of this material.");

        return *descriptors[std::distance(layouts.begin(), it)];
    }

    MaterialInstance2::DescriptorIterator MaterialInstance2::begin()
    {
        // Look for first enabled descriptor.
        for (size_t i = 0; i < descriptors.size(); i++)
            if (descriptors[i]) return DescriptorIterator(*this, i);

        return end();
    }

    MaterialInstance2::DescriptorIterator MaterialInstance2::begin() const
    {
        // Look for first enabled descriptor.
        for (size_t i = 0; i < descriptors.size(); i++)
            if (descriptors[i]) return DescriptorIterator(*this, i);

        return end();
    }

    MaterialInstance2::DescriptorIterator MaterialInstance2::end()
    {
        return DescriptorIterator(*this, descriptors.size());
    }

    MaterialInstance2::DescriptorIterator MaterialInstance2::end() const
    {
        return DescriptorIterator(*this, descriptors.size());
    }

}  // namespace sol
