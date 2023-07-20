#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/i_task.h"

namespace sol
{
    class UpdateGraphicsMaterialManagerTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateGraphicsMaterialManagerTask();

        UpdateGraphicsMaterialManagerTask(std::string                              taskName,
                                          ITaskResource<IGraphicsMaterialManager>* mtlManager,
                                          ITaskResource<uint32_t>*                 frameIdx);

        UpdateGraphicsMaterialManagerTask(const UpdateGraphicsMaterialManagerTask&) = delete;

        UpdateGraphicsMaterialManagerTask(UpdateGraphicsMaterialManagerTask&&) noexcept;

        ~UpdateGraphicsMaterialManagerTask() noexcept override;

        UpdateGraphicsMaterialManagerTask& operator=(const UpdateGraphicsMaterialManagerTask&) = delete;

        UpdateGraphicsMaterialManagerTask& operator=(UpdateGraphicsMaterialManagerTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<IGraphicsMaterialManager>* materialManager = nullptr;

        ITaskResource<uint32_t>* frameIndex = nullptr;
    };
}  // namespace sol
