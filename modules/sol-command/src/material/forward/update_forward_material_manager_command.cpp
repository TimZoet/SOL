#include "sol-command/material/forward/update_forward_material_manager_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-render/forward/forward_material_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateForwardMaterialManagerCommand::UpdateForwardMaterialManagerCommand() = default;

    UpdateForwardMaterialManagerCommand::~UpdateForwardMaterialManagerCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ForwardMaterialManager* UpdateForwardMaterialManagerCommand::getMaterialManager() const noexcept { return materialManager; }

    const uint32_t* UpdateForwardMaterialManagerCommand::getFrameIndexPtr() const noexcept { return frameIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void UpdateForwardMaterialManagerCommand::setMaterialManager(ForwardMaterialManager& manager)
    {
        commandQueue->requireNonFinalized();
        materialManager = &manager;
    }

    void UpdateForwardMaterialManagerCommand::setFrameIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        frameIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateForwardMaterialManagerCommand::finalize()
    {
        if (!materialManager) throw SolError("Cannot finalize UpdateForwardMaterialManagerCommand: materialManager not set.");
        if (!frameIndexPtr) throw SolError("Cannot finalize UpdateForwardMaterialManagerCommand: frameIndexPtr not set.");
    }

    void UpdateForwardMaterialManagerCommand::operator()() { materialManager->updateUniformBuffers(*frameIndexPtr); }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string UpdateForwardMaterialManagerCommand::getVizLabel() const { return "UpdateForwardMaterialManager"; }
}  // namespace sol
