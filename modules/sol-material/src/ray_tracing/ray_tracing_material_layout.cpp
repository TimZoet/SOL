#include "sol-material/ray_tracing/ray_tracing_material_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterialLayout::RayTracingMaterialLayout(VulkanDevice& device) : MaterialLayout(device) {}

    RayTracingMaterialLayout::~RayTracingMaterialLayout() noexcept = default;
}  // namespace sol
