#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-render/ray_tracing/fwd.h"
#include "sol-scenegraph/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class RayTracingTraverseCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingTraverseCommand();

        RayTracingTraverseCommand(const RayTracingTraverseCommand&) = delete;

        RayTracingTraverseCommand(RayTracingTraverseCommand&&) = delete;

        ~RayTracingTraverseCommand() noexcept override;

        RayTracingTraverseCommand& operator=(const RayTracingTraverseCommand&) = delete;

        RayTracingTraverseCommand& operator=(RayTracingTraverseCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] RayTracingTraverser* getTraverser() const noexcept;

        [[nodiscard]] Scenegraph* getScenegraph() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setRenderData(RayTracingRenderData& data);

        void setTraverser(RayTracingTraverser& traverse);

        void setScenegraph(Scenegraph& graph);

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string getVizLabel() const override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        RayTracingRenderData* renderData = nullptr;

        RayTracingTraverser* traverser = nullptr;

        Scenegraph* scenegraph = nullptr;
    };
}  // namespace sol