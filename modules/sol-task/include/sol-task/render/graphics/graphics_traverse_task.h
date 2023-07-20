#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-render/graphics/fwd.h"
#include "sol-scenegraph/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task.h"

namespace sol
{
    class GraphicsTraverseTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsTraverseTask();

        GraphicsTraverseTask(std::string                        taskName,
                             ITaskResource<GraphicsRenderData>* rdData,
                             ITaskResource<GraphicsTraverser>*  trav,
                             ITaskResource<Scenegraph>*         sg);

        GraphicsTraverseTask(const GraphicsTraverseTask&) = delete;

        GraphicsTraverseTask(GraphicsTraverseTask&&) noexcept;

        ~GraphicsTraverseTask() noexcept override;

        GraphicsTraverseTask& operator=(const GraphicsTraverseTask&) = delete;

        GraphicsTraverseTask& operator=(GraphicsTraverseTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<GraphicsRenderData>* renderData = nullptr;

        ITaskResource<GraphicsTraverser>* traverser = nullptr;

        ITaskResource<Scenegraph>* scenegraph = nullptr;
    };
}  // namespace sol
