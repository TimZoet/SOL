#include "sol-command/render/compute/compute_traverse_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/compute/compute_render_data.h"
#include "sol-render/compute/compute_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeTraverseCommand::ComputeTraverseCommand() = default;

    ComputeTraverseCommand::~ComputeTraverseCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ComputeTraverser* ComputeTraverseCommand::getTraverser() const noexcept { return traverser; }

    Scenegraph* ComputeTraverseCommand::getScenegraph() const noexcept { return scenegraph; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ComputeTraverseCommand::setRenderData(ComputeRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void ComputeTraverseCommand::setTraverser(ComputeTraverser& traverse)
    {
        commandQueue->requireNonFinalized();
        traverser = &traverse;
    }

    void ComputeTraverseCommand::setScenegraph(Scenegraph& graph)
    {
        commandQueue->requireNonFinalized();
        scenegraph = &graph;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void ComputeTraverseCommand::finalize()
    {
        if (!renderData) throw SolError("Cannot finalize ComputeTraverseCommand: renderData not set.");
        if (!traverser) throw SolError("Cannot finalize ComputeTraverseCommand: traverser not set.");
        if (!scenegraph) throw SolError("Cannot finalize ComputeTraverseCommand: scenegraph not set.");
    }

    void ComputeTraverseCommand::operator()()
    {
        renderData->clear();
        traverser->traverse(*scenegraph, *renderData);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string ComputeTraverseCommand::getVizLabel() const { return "ComputeTraverse"; }
}  // namespace sol
