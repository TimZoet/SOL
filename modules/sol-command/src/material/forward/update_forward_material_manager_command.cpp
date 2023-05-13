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

    const uint32_t* UpdateForwardMaterialManagerCommand::getImageIndexPtr() const noexcept { return imageIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void UpdateForwardMaterialManagerCommand::setMaterialManager(ForwardMaterialManager& manager)
    {
        commandQueue->requireNonFinalized();
        materialManager = &manager;
    }

    void UpdateForwardMaterialManagerCommand::setImageIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        imageIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateForwardMaterialManagerCommand::finalize()
    {
        if (!materialManager) throw SolError("Cannot finalize UpdateForwardMaterialManagerCommand: materialManager not set.");
        if (!imageIndexPtr) throw SolError("Cannot finalize UpdateForwardMaterialManagerCommand: imageIndexPtr not set.");
    }

    void UpdateForwardMaterialManagerCommand::operator()() { materialManager->updateUniformBuffers(*imageIndexPtr); }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string UpdateForwardMaterialManagerCommand::getVizLabel() const { return "UpdateForwardMaterialManager"; }
}  // namespace sol
