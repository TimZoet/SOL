#include "sol-task/material/ray_tracing/update_ray_tracing_material_manager_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateRayTracingMaterialManagerTask::UpdateRayTracingMaterialManagerTask() = default;

    UpdateRayTracingMaterialManagerTask::UpdateRayTracingMaterialManagerTask(
      std::string taskName, ITaskResource<IRayTracingMaterialManager>* mtlManager, ITaskResource<uint32_t>* frameIdx) :
        ITask(std::move(taskName)), materialManager(mtlManager), frameIndex(frameIdx)
    {
    }

    UpdateRayTracingMaterialManagerTask::UpdateRayTracingMaterialManagerTask(
      UpdateRayTracingMaterialManagerTask&&) noexcept = default;

    UpdateRayTracingMaterialManagerTask::~UpdateRayTracingMaterialManagerTask() noexcept = default;

    UpdateRayTracingMaterialManagerTask&
      UpdateRayTracingMaterialManagerTask::operator=(UpdateRayTracingMaterialManagerTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateRayTracingMaterialManagerTask::finalize()
    {
        if (!materialManager)
            throw SolError("Cannot finalize UpdateRayTracingMaterialManagerTask: materialManager not set.");
        if (!frameIndex) throw SolError("Cannot finalize UpdateRayTracingMaterialManagerTask: frameIndex not set.");
    }

    void UpdateRayTracingMaterialManagerTask::operator()()
    {
        //auto& rMaterialManager = **materialManager;
        //rMaterialManager.updateUniformBuffers(**frameIndex);
    }
}  // namespace sol
