#include "sol-task/material/compute/update_compute_material_manager_task.h"

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

    UpdateComputeMaterialManagerTask::UpdateComputeMaterialManagerTask() = default;

    UpdateComputeMaterialManagerTask::UpdateComputeMaterialManagerTask(
      std::string taskName, ITaskResource<IComputeMaterialManager>* mtlManager, ITaskResource<uint32_t>* frameIdx) :
        ITask(std::move(taskName)), materialManager(mtlManager), frameIndex(frameIdx)
    {
    }

    UpdateComputeMaterialManagerTask::UpdateComputeMaterialManagerTask(UpdateComputeMaterialManagerTask&&) noexcept =
      default;

    UpdateComputeMaterialManagerTask::~UpdateComputeMaterialManagerTask() noexcept = default;

    UpdateComputeMaterialManagerTask&
      UpdateComputeMaterialManagerTask::operator=(UpdateComputeMaterialManagerTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateComputeMaterialManagerTask::finalize()
    {
        if (!materialManager)
            throw SolError("Cannot finalize UpdateComputeMaterialManagerTask: materialManager not set.");
        if (!frameIndex) throw SolError("Cannot finalize UpdateComputeMaterialManagerTask: frameIndex not set.");
    }

    void UpdateComputeMaterialManagerTask::operator()()
    {
        // TODO:
        // auto& rMaterialManager = **materialManager;
        // rMaterialManager.update(**frameIndex);
    }
}  // namespace sol
