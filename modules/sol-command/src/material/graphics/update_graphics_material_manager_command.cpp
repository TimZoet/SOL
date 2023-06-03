#include "sol-command/material/graphics/update_graphics_material_manager_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-material/graphics/i_graphics_material_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UpdateGraphicsMaterialManagerCommand::UpdateGraphicsMaterialManagerCommand() = default;

    UpdateGraphicsMaterialManagerCommand::~UpdateGraphicsMaterialManagerCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IGraphicsMaterialManager* UpdateGraphicsMaterialManagerCommand::getMaterialManager() const noexcept
    {
        return materialManager;
    }

    const uint32_t* UpdateGraphicsMaterialManagerCommand::getFrameIndexPtr() const noexcept { return frameIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void UpdateGraphicsMaterialManagerCommand::setMaterialManager(IGraphicsMaterialManager& manager)
    {
        commandQueue->requireNonFinalized();
        materialManager = &manager;
    }

    void UpdateGraphicsMaterialManagerCommand::setFrameIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        frameIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void UpdateGraphicsMaterialManagerCommand::finalize()
    {
        if (!materialManager)
            throw SolError("Cannot finalize UpdateGraphicsMaterialManagerCommand: materialManager not set.");
        if (!frameIndexPtr)
            throw SolError("Cannot finalize UpdateGraphicsMaterialManagerCommand: frameIndexPtr not set.");
    }

    void UpdateGraphicsMaterialManagerCommand::operator()() { materialManager->updateUniformBuffers(*frameIndexPtr); }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string UpdateGraphicsMaterialManagerCommand::getVizLabel() const { return "UpdateGraphicsMaterialManager"; }
}  // namespace sol
