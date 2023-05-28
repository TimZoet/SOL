#include "sol-command/render/ray_tracing/ray_tracing_traverse_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/ray_tracing/ray_tracing_render_data.h"
#include "sol-render/ray_tracing/ray_tracing_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingTraverseCommand::RayTracingTraverseCommand() = default;

    RayTracingTraverseCommand::~RayTracingTraverseCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    RayTracingTraverser* RayTracingTraverseCommand::getTraverser() const noexcept { return traverser; }

    Scenegraph* RayTracingTraverseCommand::getScenegraph() const noexcept { return scenegraph; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RayTracingTraverseCommand::setRenderData(RayTracingRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void RayTracingTraverseCommand::setTraverser(RayTracingTraverser& traverse)
    {
        commandQueue->requireNonFinalized();
        traverser = &traverse;
    }

    void RayTracingTraverseCommand::setScenegraph(Scenegraph& graph)
    {
        commandQueue->requireNonFinalized();
        scenegraph = &graph;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void RayTracingTraverseCommand::finalize()
    {
        if (!renderData) throw SolError("Cannot finalize RayTracingTraverseCommand: renderData not set.");
        if (!traverser) throw SolError("Cannot finalize RayTracingTraverseCommand: traverser not set.");
        if (!scenegraph) throw SolError("Cannot finalize RayTracingTraverseCommand: scenegraph not set.");
    }

    void RayTracingTraverseCommand::operator()()
    {
        renderData->clear();
        traverser->traverse(*scenegraph, *renderData);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string RayTracingTraverseCommand::getVizLabel() const { return "RayTracingTraverse"; }
}  // namespace sol
