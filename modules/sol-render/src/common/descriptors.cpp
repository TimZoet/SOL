#include "sol-render/common/descriptors.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cassert>
#include <numeric>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_descriptor_pool.h"
#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-core/vulkan_top_level_acceleration_structure.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-material/material.h"
#include "sol-material/material_instance.h"
#include "sol-material/material_layout.h"
#include "sol-texture/image2d.h"
#include "sol-texture/texture2d.h"

namespace sol
{
    VulkanDescriptorPoolPtr createDescriptorPool(VulkanDevice& device, const MaterialLayout& layout, const size_t count)
    {
        assert(count > 0);

        VulkanDescriptorPool::Settings poolSettings;
        poolSettings.device  = device;
        poolSettings.maxSets = static_cast<uint32_t>(count);


        const auto create = [&](const size_t c, const VkDescriptorType type) {
            if (c > 0) poolSettings.poolSizes.emplace_back(type, static_cast<uint32_t>(c * count));
        };

        create(layout.getDescription().getAccelerationStructureCount(), VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);
        create(layout.getDescription().getCombinedImageSamplerCount(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        create(layout.getDescription().getStorageBufferCount(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
        create(layout.getDescription().getStorageImageCount(), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        create(layout.getDescription().getUniformBufferCount(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        return VulkanDescriptorPool::create(poolSettings);
    }

    VulkanDescriptorPoolPtr
      createDescriptorPool(VulkanDevice& device, const MaterialLayout& layout, const size_t count, const size_t set)
    {
        assert(count > 0);

        VulkanDescriptorPool::Settings poolSettings;
        poolSettings.device  = device;
        poolSettings.maxSets = static_cast<uint32_t>(count);

        const auto create = [&](const size_t c, const VkDescriptorType type) {
            if (c > 0) poolSettings.poolSizes.emplace_back(type, static_cast<uint32_t>(c * count));
        };

        create(layout.getDescription().getAccelerationStructureCount(set),
               VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);
        create(layout.getDescription().getCombinedImageSamplerCount(set), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        create(layout.getDescription().getStorageBufferCount(set), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
        create(layout.getDescription().getStorageImageCount(set), VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        create(layout.getDescription().getUniformBufferCount(set), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

        return VulkanDescriptorPool::create(poolSettings);
    }

    std::vector<VkDescriptorSet> allocateDescriptorSets(const VulkanDescriptorSetLayout& layout,
                                                        const VulkanDescriptorPool&      pool,
                                                        const size_t                     count)
    {
        // Repeat layout for each set that needs to be allocated.
        const std::vector            vkLayouts(count, layout.get());
        std::vector<VkDescriptorSet> descriptorSets(vkLayouts.size());

        // Allocate.
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = pool.get();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
        allocInfo.pSetLayouts        = vkLayouts.data();
        handleVulkanError(vkAllocateDescriptorSets(pool.getDevice().get(), &allocInfo, descriptorSets.data()));

        return descriptorSets;
    }

    void updateDescriptorSets(const std::vector<VkDescriptorSet>&                              descriptorSets,
                              const MaterialInstance&                                          materialInstance,
                              const std::vector<UniformBufferManager::UniformBufferReference>& uniformBuffers)
    {
        const auto& material                      = materialInstance.getMaterial();
        const auto& layout                        = material.getLayout();
        const auto  setIndex                      = materialInstance.getSetIndex();
        const auto  accelerationStructureBindings = layout.getAccelerationStructures(setIndex);
        const auto  combinedImageSamplerBindings  = layout.getCombinedImageSamplers(setIndex);
        const auto  storageBufferBindings         = layout.getStorageBuffers(setIndex);
        const auto  storageImageBindings          = layout.getStorageImages(setIndex);
        const auto  uniformBufferBindings         = layout.getUniformBuffers(setIndex);

        const auto perSetAccelerationStructureInfoCount = std::accumulate(
          accelerationStructureBindings.begin(),
          accelerationStructureBindings.end(),
          0,
          [](uint32_t sum, const auto& elem) { return sum + 1; });  // TODO: +elem->count once array support is added.

        const auto perSetStorageImageInfoCount = std::accumulate(
          storageImageBindings.begin(), storageImageBindings.end(), 0, [](uint32_t sum, const auto& elem) {
              return sum + 1;
          });  // TODO: +elem->count once array support is added.


        const auto perSetStorageBufferInfoCount = std::accumulate(
          storageBufferBindings.begin(), storageBufferBindings.end(), 0, [](uint32_t sum, const auto& elem) {
              return sum + 1;
          });  // TODO: +elem->count once array support is added.


        const auto perSetUniformBufferInfoCount = std::accumulate(
          uniformBufferBindings.begin(), uniformBufferBindings.end(), 0, [](uint32_t sum, const auto& elem) {
              return sum + elem.count;
          });
        const auto perSetImageInfoCount =
          std::accumulate(combinedImageSamplerBindings.begin(),
                          combinedImageSamplerBindings.end(),
                          0,
                          [](uint32_t sum, const auto& elem) { return sum + elem.count; });

        std::vector accelerationStructureInfos(
          perSetAccelerationStructureInfoCount * descriptorSets.size(),
          VkWriteDescriptorSetAccelerationStructureKHR{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR});
        std::vector imageInfos(perSetImageInfoCount * descriptorSets.size(), VkDescriptorImageInfo{});
        std::vector storageBufferInfos(perSetStorageBufferInfoCount * descriptorSets.size(), VkDescriptorBufferInfo{});
        std::vector storageImageInfos(perSetStorageImageInfoCount * descriptorSets.size(), VkDescriptorImageInfo{});
        std::vector uniformBufferInfos(perSetUniformBufferInfoCount * descriptorSets.size(), VkDescriptorBufferInfo{});

        // Create descriptor writes. Objects are laid out per set, and then per binding:
        // set<0>{ binding<0>, binding<1>, ..., binding<N> }, set<1>{...}, set<M>{ binding<0>, binding<1>, ..., binding<N>}
        const size_t bindingCount = layout.getDescription().getBindingCount();
        std::vector  descriptorWrites(bindingCount * descriptorSets.size(),
                                     VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET});

        for (size_t set = 0; set < descriptorSets.size(); set++)
        {
            size_t infoOffset = 0;
            for (size_t i = 0; i < accelerationStructureBindings.size(); i++)
            {
                const auto& binding = accelerationStructureBindings[i];
                auto& asInfo = accelerationStructureInfos[set * perSetAccelerationStructureInfoCount + infoOffset];
                auto& as     = *materialInstance.getAccelerationStructureData(binding.binding);
                asInfo.accelerationStructureCount = 1;
                asInfo.pAccelerationStructures    = &as.get();

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.pNext            = &asInfo;
                write.dstSet           = descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = 1;  // TODO: No array support yet.
                write.descriptorType   = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                write.pImageInfo       = nullptr;
                write.pBufferInfo      = nullptr;
                write.pTexelBufferView = nullptr;

                infoOffset++;  // TODO: No array support yet.
            }

            infoOffset = 0;

            for (size_t i = 0; i < combinedImageSamplerBindings.size(); i++)
            {
                const auto& binding = combinedImageSamplerBindings[i];
                for (size_t elem = 0; elem < binding.count; elem++)
                {
                    // TODO: While the shader can contain an array of textures now,
                    // each element will simply get the same handle assigned to it.
                    // The material instance class does not yet have an interface for
                    // retrieving arrays of textures.
                    auto& imageInfo       = imageInfos[set * perSetImageInfoCount + infoOffset + elem];
                    auto& texture         = *materialInstance.getCombinedImageSamplerData(binding.binding);
                    imageInfo.sampler     = texture.getSampler().get();
                    imageInfo.imageView   = texture.getImageView()->get();
                    imageInfo.imageLayout = texture.getImage()->getImageLayout();
                }

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.dstSet           = descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = binding.count;
                write.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo       = imageInfos.data() + set * perSetImageInfoCount + infoOffset;
                write.pBufferInfo      = nullptr;
                write.pTexelBufferView = nullptr;

                infoOffset += binding.count;
            }

            infoOffset = 0;

            for (size_t i = 0; i < storageBufferBindings.size(); i++)
            {
                const auto& binding    = storageBufferBindings[i];
                auto&       bufferInfo = storageBufferInfos[set * perSetStorageBufferInfoCount + infoOffset];
                auto&       buffer     = *materialInstance.getStorageBufferData(binding.binding);
                bufferInfo.buffer      = buffer.get();
                bufferInfo.offset      = 0;
                bufferInfo.range       = VK_WHOLE_SIZE;

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.dstSet           = descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = 1;  // TODO: No array support yet.
                write.descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                write.pImageInfo       = nullptr;
                write.pBufferInfo      = storageBufferInfos.data() + set * perSetStorageBufferInfoCount + infoOffset;
                write.pTexelBufferView = nullptr;

                infoOffset++;  // TODO: No array support yet.
            }

            infoOffset = 0;

            for (size_t i = 0; i < storageImageBindings.size(); i++)
            {
                const auto& binding   = storageImageBindings[i];
                auto&       imageInfo = storageImageInfos[set * perSetStorageImageInfoCount + infoOffset];
                auto&       texture   = *materialInstance.getStorageImageData(binding.binding);
                // TODO: We use Texture2D here, which always has a sampler. While we don't access it here,
                // it seems the Texture2D class always constructs a sampler. Introduce a new, sampler-less
                // image+imageview class and use that here and in materials?
                imageInfo.imageView   = texture.getImageView()->get();
                imageInfo.imageLayout = texture.getImage()->getImageLayout();

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.dstSet           = descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = 1;  // TODO: No array support yet.
                write.descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                write.pImageInfo       = storageImageInfos.data() + set * perSetStorageImageInfoCount + infoOffset;
                write.pBufferInfo      = nullptr;
                write.pTexelBufferView = nullptr;

                infoOffset++;  // TODO: No array support yet.
            }

            infoOffset = 0;

            for (size_t i = 0; i < uniformBufferBindings.size(); i++)
            {
                const auto& binding = uniformBufferBindings[i];
                const auto& [uniformBuffer, slot, offset] = uniformBuffers[i];

                for (size_t elem = 0; elem < binding.count; elem++)
                {
                    auto& bufferInfo  = uniformBufferInfos[set * perSetUniformBufferInfoCount + infoOffset + elem];
                    bufferInfo.buffer = uniformBuffer->getBuffer(set).get();
                    bufferInfo.offset = offset + elem * binding.size;
                    bufferInfo.range  = binding.size;
                }

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.dstSet           = descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = binding.count;
                write.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pImageInfo       = nullptr;
                write.pBufferInfo      = uniformBufferInfos.data() + set * perSetUniformBufferInfoCount + infoOffset;
                write.pTexelBufferView = nullptr;

                infoOffset += binding.count;
            }
        }

        vkUpdateDescriptorSets(materialInstance.getDevice().get(),
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(),
                               0,
                               nullptr);
    }
}  // namespace sol
