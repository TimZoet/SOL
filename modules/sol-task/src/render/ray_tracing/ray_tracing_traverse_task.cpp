#include "sol-task/render/ray_tracing/ray_tracing_traverse_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/ray_tracing/ray_tracing_render_data.h"
#include "sol-render/ray_tracing/ray_tracing_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingTraverseTask::RayTracingTraverseTask() = default;

    RayTracingTraverseTask::RayTracingTraverseTask(std::string                          taskName,
                                                   ITaskResource<RayTracingRenderData>* rdData,
                                                   ITaskResource<RayTracingTraverser>*  trav,
                                                   ITaskResource<Scenegraph>*           sg) :
        ITask(std::move(taskName)), renderData(rdData), traverser(trav), scenegraph(sg)
    {
    }

    RayTracingTraverseTask::RayTracingTraverseTask(RayTracingTraverseTask&&) noexcept = default;

    RayTracingTraverseTask::~RayTracingTraverseTask() noexcept = default;

    RayTracingTraverseTask& RayTracingTraverseTask::operator=(RayTracingTraverseTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void RayTracingTraverseTask::finalize()
    {
        if (!renderData) throw SolError("Cannot finalize RayTracingTraverseTask: renderData not set.");
        if (!traverser) throw SolError("Cannot finalize RayTracingTraverseTask: traverser not set.");
        if (!scenegraph) throw SolError("Cannot finalize RayTracingTraverseTask: scenegraph not set.");
    }

    void RayTracingTraverseTask::operator()()
    {
        auto&       rRenderData = **renderData;
        auto&       rTraverser  = **traverser;
        const auto& rScenegraph = **scenegraph;
        rRenderData.clear();
        rTraverser.traverse(rScenegraph, rRenderData);
    }
}  // namespace sol
