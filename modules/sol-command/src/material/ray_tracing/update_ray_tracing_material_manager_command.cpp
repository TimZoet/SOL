#include "sol-command/material/ray_tracing/update_ray_tracing_material_manager_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/ray_tracing/ray_tracing_material_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateRayTracingMaterialManagerCommand::UpdateRayTracingMaterialManagerCommand() = default;

    UpdateRayTracingMaterialManagerCommand::~UpdateRayTracingMaterialManagerCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterialManager* UpdateRayTracingMaterialManagerCommand::getMaterialManager() const noexcept
    {
        return materialManager;
    }

    const uint32_t* UpdateRayTracingMaterialManagerCommand::getFrameIndexPtr() const noexcept { return frameIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void UpdateRayTracingMaterialManagerCommand::setMaterialManager(RayTracingMaterialManager& manager)
    {
        commandQueue->requireNonFinalized();
        materialManager = &manager;
    }

    void UpdateRayTracingMaterialManagerCommand::setFrameIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        frameIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateRayTracingMaterialManagerCommand::finalize()
    {
        if (!materialManager)
            throw SolError("Cannot finalize UpdateRayTracingMaterialManagerCommand: materialManager not set.");
        if (!frameIndexPtr)
            throw SolError("Cannot finalize UpdateRayTracingMaterialManagerCommand: frameIndexPtr not set.");
    }

    void UpdateRayTracingMaterialManagerCommand::operator()() {}

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string UpdateRayTracingMaterialManagerCommand::getVizLabel() const { return "UpdateRayTracingMaterialManager"; }
}  // namespace sol
