#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"
#include "sol-material/common/uniform_buffer_manager.h"

namespace sol
{
    /**
     * \brief Create a new descriptor pool for all descriptor sets of the given material layout.
     * \param device VulkanDevice.
     * \param layout Material layout.
     * \param count Multiplier for the number of sets that can be allocated from the resulting pool. Should be at least 1.
     * \return Descriptor pool.
     */
    [[nodiscard]] VulkanDescriptorPoolPtr
      createDescriptorPool(VulkanDevice& device, const MaterialLayout& layout, size_t count);

    /**
     * \brief Create a new descriptor pool for a single descriptor set of the given material layout.
     * \param device VulkanDevice.
     * \param layout Material layout.
     * \param count Multiplier for the number of sets that can be allocated from the resulting pool. Should be at least 1.
     * \param set Descriptor set index.
     * \return Descriptor pool.
     */
    [[nodiscard]] VulkanDescriptorPoolPtr
      createDescriptorPool(VulkanDevice& device, const MaterialLayout& layout, size_t count, size_t set);

    /**
     * \brief Allocate descriptor sets from a descriptor pool.
     * \param layout Descriptor set layout.
     * \param pool Descriptor pool.
     * \param count Number of descriptor sets that is allocated at once.
     * \return List of descriptor sets.
     */
    std::vector<VkDescriptorSet>
      allocateDescriptorSets(const VulkanDescriptorSetLayout& layout, const VulkanDescriptorPool& pool, size_t count);

    /**
     * \brief Update a list of descriptor sets for a material instance.
     * \param descriptorSets List of descriptor sets.
     * \param materialInstance Material instance.
     * \param uniformBuffers List of uniform buffers.
     */
    void updateDescriptorSets(const std::vector<VkDescriptorSet>&                              descriptorSets,
                              const MaterialInstance&                                          materialInstance,
                              const std::vector<UniformBufferManager::UniformBufferReference>& uniformBuffers);
}  // namespace sol
