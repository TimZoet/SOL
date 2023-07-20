#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-render/compute/fwd.h"
#include "sol-scenegraph/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task.h"

namespace sol
{
    class ComputeTraverseTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeTraverseTask();

        ComputeTraverseTask(std::string                       taskName,
                            ITaskResource<ComputeRenderData>* rdData,
                            ITaskResource<ComputeTraverser>*  trav,
                            ITaskResource<Scenegraph>*        sg);

        ComputeTraverseTask(const ComputeTraverseTask&) = delete;

        ComputeTraverseTask(ComputeTraverseTask&&) noexcept;

        ~ComputeTraverseTask() noexcept override;

        ComputeTraverseTask& operator=(const ComputeTraverseTask&) = delete;

        ComputeTraverseTask& operator=(ComputeTraverseTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<ComputeRenderData>* renderData = nullptr;

        ITaskResource<ComputeTraverser>* traverser = nullptr;

        ITaskResource<Scenegraph>* scenegraph = nullptr;
    };
}  // namespace sol
