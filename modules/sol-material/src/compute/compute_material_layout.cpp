#include "sol-material/compute/compute_material_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialLayout::ComputeMaterialLayout(VulkanDevice& vkDevice) : MaterialLayout(vkDevice) {}

    ComputeMaterialLayout::~ComputeMaterialLayout() noexcept = default;
}  // namespace sol
