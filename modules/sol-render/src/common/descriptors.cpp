#include "sol-render/common/descriptors.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cassert>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_pool.h"
#include "sol-material/material_layout.h"

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
}  // namespace sol
