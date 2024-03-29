#include "sol-command/other/custom_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    CustomCommand::CustomCommand() = default;

    CustomCommand::~CustomCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::function<void()>& CustomCommand::getFunction() const noexcept { return function; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void CustomCommand::setFunction(std::function<void()> f)
    {
        commandQueue->requireNonFinalized();
        function = std::move(f);
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void CustomCommand::finalize()
    {
        if (!function) throw SolError("Cannot finalize CustomCommand: no function set.");
    }

    void CustomCommand::operator()() { function(); }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string CustomCommand::getVizLabel() const { return "Custom"; }
}  // namespace sol
