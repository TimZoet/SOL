#include "sol-material/compute/compute_material_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialLayout::ComputeMaterialLayout(VulkanDevice& device) : MaterialLayout(device) {}

    ComputeMaterialLayout::~ComputeMaterialLayout() noexcept = default;
}  // namespace sol
