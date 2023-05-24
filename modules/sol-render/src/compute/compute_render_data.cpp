#include "sol-render/compute/compute_render_data.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/compute/compute_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeRenderData::ComputeRenderData() = default;

    ComputeRenderData::~ComputeRenderData() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void ComputeRenderData::clear()
    {
        dispatches.clear();
        materialInstances.clear();
    }
}  // namespace sol
