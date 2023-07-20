#include "sol-task/render/graphics/graphics_traverse_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/graphics/graphics_render_data.h"
#include "sol-render/graphics/graphics_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsTraverseTask::GraphicsTraverseTask() = default;

    GraphicsTraverseTask::GraphicsTraverseTask(std::string                        taskName,
                                               ITaskResource<GraphicsRenderData>* rdData,
                                               ITaskResource<GraphicsTraverser>*  trav,
                                               ITaskResource<Scenegraph>*         sg) :
        ITask(std::move(taskName)), renderData(rdData), traverser(trav), scenegraph(sg)
    {
    }

    GraphicsTraverseTask::GraphicsTraverseTask(GraphicsTraverseTask&&) noexcept = default;

    GraphicsTraverseTask::~GraphicsTraverseTask() noexcept = default;

    GraphicsTraverseTask& GraphicsTraverseTask::operator=(GraphicsTraverseTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void GraphicsTraverseTask::finalize()
    {
        if (!renderData) throw SolError("Cannot finalize GraphicsTraverseTask: renderData not set.");
        if (!traverser) throw SolError("Cannot finalize GraphicsTraverseTask: traverser not set.");
        if (!scenegraph) throw SolError("Cannot finalize GraphicsTraverseTask: scenegraph not set.");
    }

    void GraphicsTraverseTask::operator()()
    {
        auto&       rRenderData = **renderData;
        auto&       rTraverser  = **traverser;
        const auto& rScenegraph = **scenegraph;
        rRenderData.clear();
        rTraverser.traverse(rScenegraph, rRenderData);
    }
}  // namespace sol
