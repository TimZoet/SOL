#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/i_task.h"

namespace sol
{
    class UpdateMeshManagerTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Action
        {
            Transfer   = 1,
            Deallocate = 2,
            All        = 3
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateMeshManagerTask();

        UpdateMeshManagerTask(std::string taskName, ITaskResource<MeshManager>* manager);

        UpdateMeshManagerTask(const UpdateMeshManagerTask&) = delete;

        UpdateMeshManagerTask(UpdateMeshManagerTask&&) noexcept;

        ~UpdateMeshManagerTask() noexcept override;

        UpdateMeshManagerTask& operator=(const UpdateMeshManagerTask&) = delete;

        UpdateMeshManagerTask& operator=(UpdateMeshManagerTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<MeshManager>* meshManager = nullptr;

        Action action = Action::All;
    };
}  // namespace sol
