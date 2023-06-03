#include "sol-command/render/graphics/graphics_traverse_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/graphics/graphics_render_data.h"
#include "sol-render/graphics/graphics_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsTraverseCommand::GraphicsTraverseCommand() = default;

    GraphicsTraverseCommand::~GraphicsTraverseCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    GraphicsTraverser* GraphicsTraverseCommand::getTraverser() const noexcept { return traverser; }

    Scenegraph* GraphicsTraverseCommand::getScenegraph() const noexcept { return scenegraph; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsTraverseCommand::setRenderData(GraphicsRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void GraphicsTraverseCommand::setTraverser(GraphicsTraverser& traverse)
    {
        commandQueue->requireNonFinalized();
        traverser = &traverse;
    }

    void GraphicsTraverseCommand::setScenegraph(Scenegraph& graph)
    {
        commandQueue->requireNonFinalized();
        scenegraph = &graph;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void GraphicsTraverseCommand::finalize()
    {
        if (!renderData) throw SolError("Cannot finalize GraphicsTraverseCommand: renderData not set.");
        if (!traverser) throw SolError("Cannot finalize GraphicsTraverseCommand: traverser not set.");
        if (!scenegraph) throw SolError("Cannot finalize GraphicsTraverseCommand: scenegraph not set.");
    }

    void GraphicsTraverseCommand::operator()()
    {
        renderData->clear();
        traverser->traverse(*scenegraph, *renderData);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string GraphicsTraverseCommand::getVizLabel() const { return "GraphicsTraverse"; }
}  // namespace sol
