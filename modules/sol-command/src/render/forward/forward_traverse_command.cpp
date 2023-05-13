#include "sol-command/render/forward/forward_traverse_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/forward/forward_render_data.h"
#include "sol-render/forward/forward_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardTraverseCommand::ForwardTraverseCommand() = default;

    ForwardTraverseCommand::~ForwardTraverseCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ForwardTraverser* ForwardTraverseCommand::getTraverser() const noexcept { return traverser; }

    Scenegraph* ForwardTraverseCommand::getScenegraph() const noexcept { return scenegraph; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ForwardTraverseCommand::setRenderData(ForwardRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void ForwardTraverseCommand::setTraverser(ForwardTraverser& traverse)
    {
        commandQueue->requireNonFinalized();
        traverser = &traverse;
    }

    void ForwardTraverseCommand::setScenegraph(Scenegraph& graph)
    {
        commandQueue->requireNonFinalized();
        scenegraph = &graph;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void ForwardTraverseCommand::finalize()
    {
        if (!renderData) throw SolError("Cannot finalize ForwardTraverseCommand: renderData not set.");
        if (!traverser) throw SolError("Cannot finalize ForwardTraverseCommand: traverser not set.");
        if (!scenegraph) throw SolError("Cannot finalize ForwardTraverseCommand: scenegraph not set.");
    }

    void ForwardTraverseCommand::operator()()
    {
        renderData->clear();
        traverser->traverse(*scenegraph, *renderData);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string ForwardTraverseCommand::getVizLabel() const { return "ForwardTraverse"; }
}  // namespace sol
