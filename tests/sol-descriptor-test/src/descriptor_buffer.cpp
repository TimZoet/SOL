#include "sol-descriptor-test/descriptor_buffer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-descriptor/descriptor.h"
#include "sol-descriptor/descriptor_buffer.h"
#include "sol-descriptor/descriptor_layout.h"

void DescriptorBuffer::operator()()
{
    sol::DescriptorBuffer::Settings settings{.memoryManager = &getMemoryManager()};

    // Test small buffer.
    settings.size      = 1024;
    const auto buffer0 = sol::DescriptorBuffer::create(settings);
    compareEQ(settings.size, buffer0->getBuffer().getSize());
    compareEQ(settings.size, buffer0->getBufferSize());
    compareEQ(0, buffer0->getBufferOffset());
    compareFalse(buffer0->isSubAllocation());
    compareNE(nullptr, buffer0->getVirtualBlock());

    // Test large buffer.
    settings.size      = 1024ULL * 1024 * 256;
    const auto buffer1 = sol::DescriptorBuffer::create(settings);
    compareEQ(settings.size, buffer1->getBuffer().getSize());
    compareEQ(settings.size, buffer1->getBufferSize());
    compareEQ(0, buffer1->getBufferOffset());
    compareFalse(buffer1->isSubAllocation());
    compareNE(nullptr, buffer1->getVirtualBlock());

    /*
     * Allocate descriptors from buffer0.
     */

    const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());
    expectNoThrow([&] {
        layout->add(sol::DescriptorLayout::SamplerBinding{
          .binding = 0, .count = 1, .stages = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT});
        layout->finalize();
    });


    // Should be able to allocate buffer_size / layout_size descriptors.
    std::vector<sol::DescriptorPtr> descriptors(buffer0->getBufferSize() / layout->getLayoutSize());
    expectNoThrow([&] {
        for (auto& descriptor : descriptors) descriptor = buffer0->allocateDescriptor(*layout);
    });

    // Next allocation should fail.
    expectThrow([&] { static_cast<void>(buffer0->allocateDescriptor(*layout)); });

    // Clearing a descriptor should free up space again.
    expectNoThrow([&] {
        descriptors[0].reset();
        descriptors[0] = buffer0->allocateDescriptor(*layout);
    });

    // Next allocation should fail.
    expectThrow([&] { static_cast<void>(buffer0->allocateDescriptor(*layout)); });
}
