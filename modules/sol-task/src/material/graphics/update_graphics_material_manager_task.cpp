#include "sol-task/material/graphics/update_graphics_material_manager_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-material/graphics/i_graphics_material_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateGraphicsMaterialManagerTask::UpdateGraphicsMaterialManagerTask() = default;

    UpdateGraphicsMaterialManagerTask::UpdateGraphicsMaterialManagerTask(
      std::string taskName, ITaskResource<IGraphicsMaterialManager>* mtlManager, ITaskResource<uint32_t>* frameIdx) :
        ITask(std::move(taskName)), materialManager(mtlManager), frameIndex(frameIdx)
    {
    }

    UpdateGraphicsMaterialManagerTask::UpdateGraphicsMaterialManagerTask(UpdateGraphicsMaterialManagerTask&&) noexcept =
      default;

    UpdateGraphicsMaterialManagerTask::~UpdateGraphicsMaterialManagerTask() noexcept = default;

    UpdateGraphicsMaterialManagerTask&
      UpdateGraphicsMaterialManagerTask::operator=(UpdateGraphicsMaterialManagerTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateGraphicsMaterialManagerTask::finalize()
    {
        if (!materialManager)
            throw SolError("Cannot finalize UpdateGraphicsMaterialManagerTask: materialManager not set.");
        if (!frameIndex) throw SolError("Cannot finalize UpdateGraphicsMaterialManagerTask: frameIndex not set.");
    }

    void UpdateGraphicsMaterialManagerTask::operator()()
    {
        auto& rMaterialManager = **materialManager;
        rMaterialManager.updateUniformBuffers(**frameIndex);
    }
}  // namespace sol
