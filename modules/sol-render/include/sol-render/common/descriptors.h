#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"

namespace sol
{
    /**
     * \brief Create a new descriptor pool for all descriptor sets of the given material layout.
     * \param device VulkanDevice.
     * \param layout Material layout.
     * \param count Number of materials that can be allocated from this pool. Should be at least 1.
     * \return Descriptor pool.
     */
    [[nodiscard]] VulkanDescriptorPoolPtr
      createDescriptorPool(VulkanDevice& device, const MaterialLayout& layout, size_t count);

    /**
     * \brief Create a new descriptor pool for a single descriptor set of the given material layout.
     * \param device VulkanDevice.
     * \param layout Material layout.
     * \param count Number of materials that can be allocated from this pool. Should be at least 1.
     * \param set Descriptor set index.
     * \return Descriptor pool.
     */
    [[nodiscard]] VulkanDescriptorPoolPtr
      createDescriptorPool(VulkanDevice& device, const MaterialLayout& layout, size_t count, size_t set);
}  // namespace sol
