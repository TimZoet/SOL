#include "sol-task/texture/update_texture_manager_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-texture/texture_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateTextureManagerTask::UpdateTextureManagerTask() = default;

    UpdateTextureManagerTask::UpdateTextureManagerTask(std::string taskName, ITaskResource<TextureManager>* manager) :
        ITask(std::move(taskName)), textureManager(manager)
    {
    }

    UpdateTextureManagerTask::UpdateTextureManagerTask(UpdateTextureManagerTask&&) noexcept = default;

    UpdateTextureManagerTask::~UpdateTextureManagerTask() noexcept = default;

    UpdateTextureManagerTask& UpdateTextureManagerTask::operator=(UpdateTextureManagerTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateTextureManagerTask::finalize()
    {
        if (!textureManager) throw SolError("Cannot finalize UpdateTextureManagerTask: textureManager not set.");
    }

    void UpdateTextureManagerTask::operator()() { (*textureManager)->transfer(); }
}  // namespace sol
