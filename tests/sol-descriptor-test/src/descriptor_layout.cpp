#include "sol-descriptor-test/descriptor_layout.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/descriptor_layout.h"

void DescriptorLayout::operator()()
{
    // Test initial state.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        expectThrow([&] { static_cast<void>(layout->getLayoutSize()); });
        expectThrow([&] { layout->requireFinalized(); });
        expectNoThrow([&] { layout->requireNonFinalized(); });
        expectNoThrow([&] { static_cast<void>(layout->getAccelerationStructures()); });
        expectNoThrow([&] { static_cast<void>(layout->getSampledImages()); });
        expectNoThrow([&] { static_cast<void>(layout->getSamplers()); });
        expectNoThrow([&] { static_cast<void>(layout->getStorageBuffers()); });
        expectNoThrow([&] { static_cast<void>(layout->getStorageImages()); });
        expectNoThrow([&] { static_cast<void>(layout->getUniformBuffers()); });
        expectThrow([&] { static_cast<void>(layout->getAccelerationStructure(0)); });
        expectThrow([&] { static_cast<void>(layout->getSampledImage(0)); });
        expectThrow([&] { static_cast<void>(layout->getSampler(0)); });
        expectThrow([&] { static_cast<void>(layout->getStorageBuffer(0)); });
        expectThrow([&] { static_cast<void>(layout->getStorageImage(0)); });
        expectThrow([&] { static_cast<void>(layout->getUniformBuffer(0)); });
    }

    // TODO: Re-enable once extensions are enabled.
    // Test AccelerationStructureBinding.
    /*{
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        const std::vector bindings = {
          sol::DescriptorLayout::AccelerationStructureBinding{
            .binding = 0, .count = 1, .stages = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR},
          sol::DescriptorLayout::AccelerationStructureBinding{
            .binding = 1, .count = 1, .stages = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR}};

        expectNoThrow([&] { layout->add(bindings[1]); });
        expectNoThrow([&] { layout->add(bindings[0]); });
        expectNoThrow([&] { static_cast<void>(layout->getAccelerationStructure(0)); });
        expectNoThrow([&] { static_cast<void>(layout->getAccelerationStructure(1)); });
        expectThrow([&] { static_cast<void>(layout->getAccelerationStructure(2)); });
        expectNoThrow([&] { layout->finalize(); });
        expectNoThrow([&] { layout->requireFinalized(); });
        expectThrow([&] { layout->requireNonFinalized(); });
        compareEQ(bindings, layout->getAccelerationStructures());
        compareGT(layout->getLayoutSize(), 0);
    }*/

    // Test SampledImageBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        const std::vector bindings = {sol::DescriptorLayout::SampledImageBinding{
                                        .binding = 0, .count = 10, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT},
                                      sol::DescriptorLayout::SampledImageBinding{
                                        .binding = 1, .count = 22, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT}};

        expectNoThrow([&] { layout->add(bindings[1]); });
        expectNoThrow([&] { layout->add(bindings[0]); });
        expectNoThrow([&] { static_cast<void>(layout->getSampledImage(0)); });
        expectNoThrow([&] { static_cast<void>(layout->getSampledImage(1)); });
        expectThrow([&] { static_cast<void>(layout->getSampledImage(2)); });
        expectNoThrow([&] { layout->finalize(); });
        expectNoThrow([&] { layout->requireFinalized(); });
        expectThrow([&] { layout->requireNonFinalized(); });
        compareEQ(bindings, layout->getSampledImages());
        compareGT(layout->getLayoutSize(), 0);
    }

    // Test SamplerBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        const std::vector bindings = {sol::DescriptorLayout::SamplerBinding{
                                        .binding = 0, .count = 1, .stages = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT},
                                      sol::DescriptorLayout::SamplerBinding{
                                        .binding = 1, .count = 4, .stages = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT}};

        expectNoThrow([&] { layout->add(bindings[1]); });
        expectNoThrow([&] { layout->add(bindings[0]); });
        expectNoThrow([&] { static_cast<void>(layout->getSampler(0)); });
        expectNoThrow([&] { static_cast<void>(layout->getSampler(1)); });
        expectThrow([&] { static_cast<void>(layout->getSampler(2)); });
        expectNoThrow([&] { layout->finalize(); });
        expectNoThrow([&] { layout->requireFinalized(); });
        expectThrow([&] { layout->requireNonFinalized(); });
        compareEQ(bindings, layout->getSamplers());
        compareGT(layout->getLayoutSize(), 0);
    }

    // Test StorageBufferBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        const std::vector bindings = {sol::DescriptorLayout::StorageBufferBinding{
                                        .binding = 0, .count = 2, .stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT},
                                      sol::DescriptorLayout::StorageBufferBinding{
                                        .binding = 1, .count = 1, .stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}};

        expectNoThrow([&] { layout->add(bindings[1]); });
        expectNoThrow([&] { layout->add(bindings[0]); });
        expectNoThrow([&] { static_cast<void>(layout->getStorageBuffer(0)); });
        expectNoThrow([&] { static_cast<void>(layout->getStorageBuffer(1)); });
        expectThrow([&] { static_cast<void>(layout->getStorageBuffer(2)); });
        expectNoThrow([&] { layout->finalize(); });
        expectNoThrow([&] { layout->requireFinalized(); });
        expectThrow([&] { layout->requireNonFinalized(); });
        compareEQ(bindings, layout->getStorageBuffers());
        compareGT(layout->getLayoutSize(), 0);
    }

    // Test StorageImageBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        const std::vector bindings = {sol::DescriptorLayout::StorageImageBinding{
                                        .binding = 0, .count = 2, .stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT},
                                      sol::DescriptorLayout::StorageImageBinding{
                                        .binding = 1, .count = 1, .stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}};

        expectNoThrow([&] { layout->add(bindings[1]); });
        expectNoThrow([&] { layout->add(bindings[0]); });
        expectNoThrow([&] { static_cast<void>(layout->getStorageImage(0)); });
        expectNoThrow([&] { static_cast<void>(layout->getStorageImage(1)); });
        expectThrow([&] { static_cast<void>(layout->getStorageImage(2)); });
        expectNoThrow([&] { layout->finalize(); });
        expectNoThrow([&] { layout->requireFinalized(); });
        expectThrow([&] { layout->requireNonFinalized(); });
        compareEQ(bindings, layout->getStorageImages());
        compareGT(layout->getLayoutSize(), 0);
    }

    // Test UniformBufferBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        const std::vector bindings = {
          sol::DescriptorLayout::UniformBufferBinding{
            .binding = 0, .size = 1024, .count = 1, .stages = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT},
          sol::DescriptorLayout::UniformBufferBinding{
            .binding = 1, .size = 8, .count = 16, .stages = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT}};

        expectNoThrow([&] { layout->add(bindings[1]); });
        expectNoThrow([&] { layout->add(bindings[0]); });
        expectNoThrow([&] { static_cast<void>(layout->getUniformBuffer(0)); });
        expectNoThrow([&] { static_cast<void>(layout->getUniformBuffer(1)); });
        expectThrow([&] { static_cast<void>(layout->getUniformBuffer(2)); });
        expectNoThrow([&] { layout->finalize(); });
        expectNoThrow([&] { layout->requireFinalized(); });
        expectThrow([&] { layout->requireNonFinalized(); });
        compareEQ(bindings, layout->getUniformBuffers());
        compareGT(layout->getLayoutSize(), 0);
    }

    // TODO: Also test:
    // - overlap
    // - gaps
    // - more complex bindings
}
