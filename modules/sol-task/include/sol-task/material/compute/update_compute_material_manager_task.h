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
    class UpdateComputeMaterialManagerTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateComputeMaterialManagerTask();

        UpdateComputeMaterialManagerTask(std::string                             taskName,
                                         ITaskResource<IComputeMaterialManager>* mtlManager,
                                         ITaskResource<uint32_t>*                frameIdx);

        UpdateComputeMaterialManagerTask(const UpdateComputeMaterialManagerTask&) = delete;

        UpdateComputeMaterialManagerTask(UpdateComputeMaterialManagerTask&&) noexcept;

        ~UpdateComputeMaterialManagerTask() noexcept override;

        UpdateComputeMaterialManagerTask& operator=(const UpdateComputeMaterialManagerTask&) = delete;

        UpdateComputeMaterialManagerTask& operator=(UpdateComputeMaterialManagerTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<IComputeMaterialManager>* materialManager = nullptr;

        ITaskResource<uint32_t>* frameIndex = nullptr;
    };
}  // namespace sol
