#include "sol-descriptor-test/descriptor.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-descriptor/descriptor.h"
#include "sol-descriptor/descriptor_buffer.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/image2d2.h"
#include "sol-texture/texture2d2.h"

void Descriptor::operator()()
{
    // Create descriptor buffer that should be able to hold enough descriptors for these tests.
    const sol::DescriptorBuffer::Settings settings{.memoryManager = &getMemoryManager(), .size = 1024ULL * 10244};
    const auto                            buffer = sol::DescriptorBuffer::create(settings);

    /*
     * Create a bunch of resources to assign.
     */

    auto image2D =
      sol::Image2D2::create(sol::Image2D2::Settings{.memoryManager = getMemoryManager(),
                                                    .size          = {32u, 32u},
                                                    .format        = VK_FORMAT_R8G8B8A8_SRGB,
                                                    .levels        = 1,
                                                    .usage         = VK_IMAGE_USAGE_SAMPLED_BIT,
                                                    .aspect        = VK_IMAGE_ASPECT_COLOR_BIT,
                                                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                    .initialOwner  = getMemoryManager().getGraphicsQueue().getFamily(),
                                                    .tiling        = VK_IMAGE_TILING_OPTIMAL});
    auto sampler2D = sol::Sampler2D::create(sol::Sampler2D::Settings{.device = getDevice()});
    auto texture2D = sol::Texture2D2::create(sol::Texture2D2::Settings{.image = *image2D, .sampler = *sampler2D});

    sol::IBufferAllocator::AllocationInfo alloc{.size        = 1024,
                                                .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                               VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                                                .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
                                                .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
                                                .requiredMemoryFlags  = 0,
                                                .preferredMemoryFlags = 0,
                                                .allocationFlags      = 0,
                                                .alignment            = 0};

    const auto storageBuffer =
      getMemoryManager().allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);

    alloc.bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    const auto uniformBuffer =
      getMemoryManager().allocateBuffer(alloc, sol::IBufferAllocator::OnAllocationFailure::Throw);

    // TODO: Re-enable once extensions are enabled.
    // Test AccelerationStructureBinding.
    /*{
        
    }*/

    // Test SampledImageBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        expectNoThrow([&] {
            layout->add(sol::DescriptorLayout::SampledImageBinding{
              .binding = 0, .count = 1, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->add(sol::DescriptorLayout::SampledImageBinding{
              .binding = 1, .count = 4, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->finalize();
        });

        const auto descriptor = buffer->allocateDescriptor(*layout);

        expectNoThrow([&] { descriptor->setSampledImage(texture2D->getImageView(), 0, 0); });
        expectNoThrow([&] { descriptor->setSampledImage(texture2D->getImageView(), 1, 0); });
        expectNoThrow([&] { descriptor->setSampledImage(texture2D->getImageView(), 1, 1); });
        expectNoThrow([&] { descriptor->setSampledImage(texture2D->getImageView(), 1, 2); });
        expectNoThrow([&] { descriptor->setSampledImage(texture2D->getImageView(), 1, 3); });
        expectThrow([&] { descriptor->setSampledImage(texture2D->getImageView(), 1, 4); });
        expectThrow([&] { descriptor->setSampledImage(texture2D->getImageView(), 2, 0); });

        expectNoThrow([&] { descriptor->setSampledImage(*texture2D, 0, 0); });
        expectNoThrow([&] { descriptor->setSampledImage(*texture2D, 1, 0); });
        expectNoThrow([&] { descriptor->setSampledImage(*texture2D, 1, 1); });
        expectNoThrow([&] { descriptor->setSampledImage(*texture2D, 1, 2); });
        expectNoThrow([&] { descriptor->setSampledImage(*texture2D, 1, 3); });
        expectThrow([&] { descriptor->setSampledImage(*texture2D, 1, 4); });
        expectThrow([&] { descriptor->setSampledImage(*texture2D, 2, 0); });

        std::array<const sol::VulkanImageView*, 6> views{&texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView()};
        expectNoThrow([&] { descriptor->setSampledImageRange(std::span(views.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setSampledImageRange(std::span(views.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setSampledImageRange(std::span(views.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setSampledImageRange(std::span(views.begin() + 1, 5), 1, 0); });

        std::array<const sol::Texture2D2*, 6> textures{
          texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get()};
        expectNoThrow([&] { descriptor->setSampledImageRange(std::span(textures.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setSampledImageRange(std::span(textures.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setSampledImageRange(std::span(textures.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setSampledImageRange(std::span(textures.begin() + 1, 5), 1, 0); });
    }

    // Test SamplerBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        expectNoThrow([&] {
            layout->add(sol::DescriptorLayout::SamplerBinding{
              .binding = 0, .count = 1, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->add(sol::DescriptorLayout::SamplerBinding{
              .binding = 1, .count = 4, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->finalize();
        });

        const auto descriptor = buffer->allocateDescriptor(*layout);

        expectNoThrow([&] { descriptor->setSampler(sampler2D->getSampler(), 0, 0); });
        expectNoThrow([&] { descriptor->setSampler(sampler2D->getSampler(), 1, 0); });
        expectNoThrow([&] { descriptor->setSampler(sampler2D->getSampler(), 1, 1); });
        expectNoThrow([&] { descriptor->setSampler(sampler2D->getSampler(), 1, 2); });
        expectNoThrow([&] { descriptor->setSampler(sampler2D->getSampler(), 1, 3); });
        expectThrow([&] { descriptor->setSampler(sampler2D->getSampler(), 1, 4); });
        expectThrow([&] { descriptor->setSampler(sampler2D->getSampler(), 2, 0); });

        expectNoThrow([&] { descriptor->setSampler(*sampler2D, 0, 0); });
        expectNoThrow([&] { descriptor->setSampler(*sampler2D, 1, 0); });
        expectNoThrow([&] { descriptor->setSampler(*sampler2D, 1, 1); });
        expectNoThrow([&] { descriptor->setSampler(*sampler2D, 1, 2); });
        expectNoThrow([&] { descriptor->setSampler(*sampler2D, 1, 3); });
        expectThrow([&] { descriptor->setSampler(*sampler2D, 1, 4); });
        expectThrow([&] { descriptor->setSampler(*sampler2D, 2, 0); });

        expectNoThrow([&] { descriptor->setSampler(*texture2D, 0, 0); });
        expectNoThrow([&] { descriptor->setSampler(*texture2D, 1, 0); });
        expectNoThrow([&] { descriptor->setSampler(*texture2D, 1, 1); });
        expectNoThrow([&] { descriptor->setSampler(*texture2D, 1, 2); });
        expectNoThrow([&] { descriptor->setSampler(*texture2D, 1, 3); });
        expectThrow([&] { descriptor->setSampler(*texture2D, 1, 4); });
        expectThrow([&] { descriptor->setSampler(*texture2D, 2, 0); });

        std::array<const sol::VulkanSampler*, 6> vsamplers{&sampler2D->getSampler(),
                                                           &sampler2D->getSampler(),
                                                           &sampler2D->getSampler(),
                                                           &sampler2D->getSampler(),
                                                           &sampler2D->getSampler(),
                                                           &sampler2D->getSampler()};
        expectNoThrow([&] { descriptor->setSamplerRange(std::span(vsamplers.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setSamplerRange(std::span(vsamplers.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setSamplerRange(std::span(vsamplers.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setSamplerRange(std::span(vsamplers.begin() + 1, 5), 1, 0); });

        std::array<const sol::Sampler2D*, 6> samplers{
          sampler2D.get(), sampler2D.get(), sampler2D.get(), sampler2D.get(), sampler2D.get(), sampler2D.get()};
        expectNoThrow([&] { descriptor->setSamplerRange(std::span(samplers.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setSamplerRange(std::span(samplers.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setSamplerRange(std::span(samplers.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setSamplerRange(std::span(samplers.begin() + 1, 5), 1, 0); });

        std::array<const sol::Texture2D2*, 6> textures{
          texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get()};
        expectNoThrow([&] { descriptor->setSamplerRange(std::span(textures.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setSamplerRange(std::span(textures.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setSamplerRange(std::span(textures.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setSamplerRange(std::span(textures.begin() + 1, 5), 1, 0); });
    }

    // Test StorageBufferBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        expectNoThrow([&] {
            layout->add(sol::DescriptorLayout::StorageBufferBinding{
              .binding = 0, .count = 1, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->add(sol::DescriptorLayout::StorageBufferBinding{
              .binding = 1, .count = 4, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->finalize();
        });

        const auto descriptor = buffer->allocateDescriptor(*layout);

        expectNoThrow([&] { descriptor->setStorageBuffer(*storageBuffer, 0, 0); });
        expectNoThrow([&] { descriptor->setStorageBuffer(*storageBuffer, 1, 0); });
        expectNoThrow([&] { descriptor->setStorageBuffer(*storageBuffer, 1, 1); });
        expectNoThrow([&] { descriptor->setStorageBuffer(*storageBuffer, 1, 2); });
        expectNoThrow([&] { descriptor->setStorageBuffer(*storageBuffer, 1, 3); });
        expectThrow([&] { descriptor->setStorageBuffer(*storageBuffer, 1, 4); });
        expectThrow([&] { descriptor->setStorageBuffer(*storageBuffer, 2, 0); });

        std::array<const sol::IBuffer*, 6> buffers{storageBuffer.get(),
                                                   storageBuffer.get(),
                                                   storageBuffer.get(),
                                                   storageBuffer.get(),
                                                   storageBuffer.get(),
                                                   storageBuffer.get()};
        expectNoThrow([&] { descriptor->setStorageBufferRange(std::span(buffers.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setStorageBufferRange(std::span(buffers.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setStorageBufferRange(std::span(buffers.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setStorageBufferRange(std::span(buffers.begin() + 1, 5), 1, 0); });
    }

    // Test StorageImageBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        expectNoThrow([&] {
            layout->add(sol::DescriptorLayout::StorageImageBinding{
              .binding = 0, .count = 1, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->add(sol::DescriptorLayout::StorageImageBinding{
              .binding = 1, .count = 4, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->finalize();
        });

        const auto descriptor = buffer->allocateDescriptor(*layout);

        expectNoThrow([&] { descriptor->setStorageImage(texture2D->getImageView(), 0, 0); });
        expectNoThrow([&] { descriptor->setStorageImage(texture2D->getImageView(), 1, 0); });
        expectNoThrow([&] { descriptor->setStorageImage(texture2D->getImageView(), 1, 1); });
        expectNoThrow([&] { descriptor->setStorageImage(texture2D->getImageView(), 1, 2); });
        expectNoThrow([&] { descriptor->setStorageImage(texture2D->getImageView(), 1, 3); });
        expectThrow([&] { descriptor->setStorageImage(texture2D->getImageView(), 1, 4); });
        expectThrow([&] { descriptor->setStorageImage(texture2D->getImageView(), 2, 0); });

        expectNoThrow([&] { descriptor->setStorageImage(*texture2D, 0, 0); });
        expectNoThrow([&] { descriptor->setStorageImage(*texture2D, 1, 0); });
        expectNoThrow([&] { descriptor->setStorageImage(*texture2D, 1, 1); });
        expectNoThrow([&] { descriptor->setStorageImage(*texture2D, 1, 2); });
        expectNoThrow([&] { descriptor->setStorageImage(*texture2D, 1, 3); });
        expectThrow([&] { descriptor->setStorageImage(*texture2D, 1, 4); });
        expectThrow([&] { descriptor->setStorageImage(*texture2D, 2, 0); });

        std::array<const sol::VulkanImageView*, 6> views{&texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView(),
                                                         &texture2D->getImageView()};
        expectNoThrow([&] { descriptor->setStorageImageRange(std::span(views.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setStorageImageRange(std::span(views.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setStorageImageRange(std::span(views.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setStorageImageRange(std::span(views.begin() + 1, 5), 1, 0); });

        std::array<const sol::Texture2D2*, 6> textures{
          texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get(), texture2D.get()};
        expectNoThrow([&] { descriptor->setStorageImageRange(std::span(textures.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setStorageImageRange(std::span(textures.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setStorageImageRange(std::span(textures.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setStorageImageRange(std::span(textures.begin() + 1, 5), 1, 0); });
    }

    // Test UniformBufferBinding.
    {
        const auto layout = std::make_unique<sol::DescriptorLayout>(getDevice());

        expectNoThrow([&] {
            layout->add(sol::DescriptorLayout::UniformBufferBinding{
              .binding = 0, .size = 1024, .count = 1, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->add(sol::DescriptorLayout::UniformBufferBinding{
              .binding = 1, .size = 1024, .count = 4, .stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT});
            layout->finalize();
        });

        const auto descriptor = buffer->allocateDescriptor(*layout);

        expectNoThrow([&] { descriptor->setUniformBuffer(*uniformBuffer, 0, 0); });
        expectNoThrow([&] { descriptor->setUniformBuffer(*uniformBuffer, 1, 0); });
        expectNoThrow([&] { descriptor->setUniformBuffer(*uniformBuffer, 1, 1); });
        expectNoThrow([&] { descriptor->setUniformBuffer(*uniformBuffer, 1, 2); });
        expectNoThrow([&] { descriptor->setUniformBuffer(*uniformBuffer, 1, 3); });
        expectThrow([&] { descriptor->setUniformBuffer(*uniformBuffer, 1, 4); });
        expectThrow([&] { descriptor->setUniformBuffer(*uniformBuffer, 2, 0); });

        std::array<const sol::IBuffer*, 6> buffers{uniformBuffer.get(),
                                                   uniformBuffer.get(),
                                                   uniformBuffer.get(),
                                                   uniformBuffer.get(),
                                                   uniformBuffer.get(),
                                                   uniformBuffer.get()};
        expectNoThrow([&] { descriptor->setUniformBufferRange(std::span(buffers.begin(), 4), 1, 0); });
        expectNoThrow([&] { descriptor->setUniformBufferRange(std::span(buffers.begin() + 1, 3), 1, 1); });
        expectThrow([&] { descriptor->setUniformBufferRange(std::span(buffers.begin() + 1, 3), 1, 2); });
        expectThrow([&] { descriptor->setUniformBufferRange(std::span(buffers.begin() + 1, 5), 1, 0); });
    }
}
