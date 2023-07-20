#include "sol-task/mesh/update_mesh_manager_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-error/sol_error.h"
#include "sol-mesh/mesh_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateMeshManagerTask::UpdateMeshManagerTask() = default;

    UpdateMeshManagerTask::UpdateMeshManagerTask(std::string taskName, ITaskResource<MeshManager>* manager) :
        ITask(std::move(taskName)), meshManager(manager)
    {
    }

    UpdateMeshManagerTask::UpdateMeshManagerTask(UpdateMeshManagerTask&&) noexcept = default;

    UpdateMeshManagerTask::~UpdateMeshManagerTask() noexcept = default;

    UpdateMeshManagerTask& UpdateMeshManagerTask::operator=(UpdateMeshManagerTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateMeshManagerTask::finalize()
    {
        if (!meshManager) throw SolError("Cannot finalize UpdateMeshManagerTask: meshManager not set.");
    }

    void UpdateMeshManagerTask::operator()()
    {
        auto& rMeshManager = **meshManager;
        if (any(action & Action::Deallocate)) rMeshManager.deallocateDeletedMeshes();
        if (any(action & Action::Transfer)) rMeshManager.transferStagedCopies();
    }
}  // namespace sol
