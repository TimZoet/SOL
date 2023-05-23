#include "sol-command/material/compute/update_compute_material_manager_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/compute/compute_material_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateComputeMaterialManagerCommand::UpdateComputeMaterialManagerCommand() = default;

    UpdateComputeMaterialManagerCommand::~UpdateComputeMaterialManagerCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialManager* UpdateComputeMaterialManagerCommand::getMaterialManager() const noexcept
    {
        return materialManager;
    }

    const uint32_t* UpdateComputeMaterialManagerCommand::getFrameIndexPtr() const noexcept { return frameIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void UpdateComputeMaterialManagerCommand::setMaterialManager(ComputeMaterialManager& manager)
    {
        commandQueue->requireNonFinalized();
        materialManager = &manager;
    }

    void UpdateComputeMaterialManagerCommand::setFrameIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        frameIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateComputeMaterialManagerCommand::finalize()
    {
        if (!materialManager)
            throw SolError("Cannot finalize UpdateComputeMaterialManagerCommand: materialManager not set.");
        if (!frameIndexPtr)
            throw SolError("Cannot finalize UpdateComputeMaterialManagerCommand: frameIndexPtr not set.");
    }

    void UpdateComputeMaterialManagerCommand::operator()() {}

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string UpdateComputeMaterialManagerCommand::getVizLabel() const { return "UpdateComputeMaterialManager"; }
}  // namespace sol
