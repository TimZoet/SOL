#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-render/ray_tracing/fwd.h"
#include "sol-scenegraph/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task.h"

namespace sol
{
    class RayTracingTraverseTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingTraverseTask();

        RayTracingTraverseTask(std::string                          taskName,
                               ITaskResource<RayTracingRenderData>* rdData,
                               ITaskResource<RayTracingTraverser>*  trav,
                               ITaskResource<Scenegraph>*           sg);

        RayTracingTraverseTask(const RayTracingTraverseTask&) = delete;

        RayTracingTraverseTask(RayTracingTraverseTask&&) noexcept;

        ~RayTracingTraverseTask() noexcept override;

        RayTracingTraverseTask& operator=(const RayTracingTraverseTask&) = delete;

        RayTracingTraverseTask& operator=(RayTracingTraverseTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<RayTracingRenderData>* renderData = nullptr;

        ITaskResource<RayTracingTraverser>* traverser = nullptr;

        ITaskResource<Scenegraph>* scenegraph = nullptr;
    };
}  // namespace sol
