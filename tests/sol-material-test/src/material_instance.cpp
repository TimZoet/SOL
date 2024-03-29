#include "sol-material-test/material_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-descriptor/descriptor.h"
#include "sol-descriptor/descriptor_buffer.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-material/graphics/graphics_material_instance2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "testutils/materials.h"

void MaterialInstance::operator()()
{
    // Create small buffer for descriptors below.
    const sol::DescriptorBuffer::Settings settings{.memoryManager = &getMemoryManager(), .size = 2048};
    const auto                            buffer = sol::DescriptorBuffer::create(settings);

    const auto [descriptorLayouts, material] = Materials::load(Materials::Graphics::Name::Simple, getDevice());

    // Create material instance.
    sol::MaterialInstance2Ptr instance;
    expectNoThrow([&] {
        instance = std::make_unique<sol::GraphicsMaterialInstance2>(uuids::uuid_system_generator{}(), *material);
    });
    compareEQ(&getDevice(), &instance->getDevice());
    compareNE(uuids::uuid{}, instance->getUuid());
    compareEQ(std::string(""), instance->getName());
    compareEQ(material.get(), &instance->getMaterial());

    // Check default disabled state of descriptors.
    expectThrow([&] { static_cast<void>(instance->operator[](0)); });
    expectThrow([&] { static_cast<void>(instance->operator[](1)); });
    expectThrow([&] { static_cast<void>(instance->operator[](2)); });
    expectNoThrow([&] {
        size_t count = 0;
        for (auto [desc, index] : *instance) count++;
        compareEQ(0, count);
    });

    // Enable descriptor at index 1.
    expectNoThrow([&] {
        instance->enableDescriptor(1, *buffer);
        // Enabling already enabled descriptor should be fine.
        instance->enableDescriptor(1, *buffer);
    });
    expectThrow([&] { static_cast<void>(instance->operator[](0)); });
    expectNoThrow([&] { static_cast<void>(instance->operator[](1)); });

    // Iterate over descriptors.
    expectNoThrow([&] {
        for (auto [desc, index] : *instance)
        {
            compareEQ(&instance->operator[](1), &desc);
            compareEQ(descriptorLayouts[1].get(), &desc.getLayout());
            compareEQ(1, index);
        }
    });

    // Enable descriptor at index 0.
    expectNoThrow([&] { instance->enableDescriptor(0, *buffer); });
    expectNoThrow([&] { static_cast<void>(instance->operator[](0)); });
    expectNoThrow([&] { static_cast<void>(instance->operator[](1)); });

    // Iterate over descriptors.
    expectNoThrow([&] {
        bool first = true;
        for (auto [desc, index] : *instance)
        {
            if (first)
            {
                compareEQ(&instance->operator[](0), &desc);
                compareEQ(descriptorLayouts[0].get(), &desc.getLayout());
                compareEQ(0, index);
            }
            else
            {
                compareEQ(&instance->operator[](1), &desc);
                compareEQ(descriptorLayouts[1].get(), &desc.getLayout());
                compareEQ(1, index);
            }
            first = false;
        }
    });

    // Disable descriptor at index 1.
    expectNoThrow([&] {
        instance->disableDescriptor(1);
        instance->disableDescriptor(1);
    });
    expectNoThrow([&] { static_cast<void>(instance->operator[](0)); });

    // Disable descriptor at index 0.
    expectNoThrow([&] { instance->disableDescriptor(0); });
    expectThrow([&] { static_cast<void>(instance->operator[](0)); });
    expectThrow([&] { static_cast<void>(instance->operator[](1)); });
}
