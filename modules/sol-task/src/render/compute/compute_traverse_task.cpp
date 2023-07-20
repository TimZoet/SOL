#include "sol-task/render/compute/compute_traverse_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/compute/compute_render_data.h"
#include "sol-render/compute/compute_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeTraverseTask::ComputeTraverseTask() = default;

    ComputeTraverseTask::ComputeTraverseTask(std::string                       taskName,
                                             ITaskResource<ComputeRenderData>* rdData,
                                             ITaskResource<ComputeTraverser>*  trav,
                                             ITaskResource<Scenegraph>*        sg) :
        ITask(std::move(taskName)), renderData(rdData), traverser(trav), scenegraph(sg)
    {
    }

    ComputeTraverseTask::ComputeTraverseTask(ComputeTraverseTask&&) noexcept = default;

    ComputeTraverseTask::~ComputeTraverseTask() noexcept = default;

    ComputeTraverseTask& ComputeTraverseTask::operator=(ComputeTraverseTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void ComputeTraverseTask::finalize()
    {
        if (!renderData) throw SolError("Cannot finalize ComputeTraverseTask: renderData not set.");
        if (!traverser) throw SolError("Cannot finalize ComputeTraverseTask: traverser not set.");
        if (!scenegraph) throw SolError("Cannot finalize ComputeTraverseTask: scenegraph not set.");
    }

    void ComputeTraverseTask::operator()()
    {
        auto&       rRenderData = **renderData;
        auto&       rTraverser  = **traverser;
        const auto& rScenegraph = **scenegraph;
        rRenderData.clear();
        rTraverser.traverse(rScenegraph, rRenderData);
    }
}  // namespace sol
