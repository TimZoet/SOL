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
    class UpdateRayTracingMaterialManagerTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateRayTracingMaterialManagerTask();

        UpdateRayTracingMaterialManagerTask(std::string                                taskName,
                                            ITaskResource<IRayTracingMaterialManager>* mtlManager,
                                            ITaskResource<uint32_t>*                   frameIdx);

        UpdateRayTracingMaterialManagerTask(const UpdateRayTracingMaterialManagerTask&) = delete;

        UpdateRayTracingMaterialManagerTask(UpdateRayTracingMaterialManagerTask&&) noexcept;

        ~UpdateRayTracingMaterialManagerTask() noexcept override;

        UpdateRayTracingMaterialManagerTask& operator=(const UpdateRayTracingMaterialManagerTask&) = delete;

        UpdateRayTracingMaterialManagerTask& operator=(UpdateRayTracingMaterialManagerTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<IRayTracingMaterialManager>* materialManager = nullptr;

        ITaskResource<uint32_t>* frameIndex = nullptr;
    };
}  // namespace sol
