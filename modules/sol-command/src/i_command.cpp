#include "sol-command/i_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::string& ICommand::getName() const noexcept { return name; }

    CommandQueue& ICommand::getCommandQueue() noexcept { return *commandQueue; }

    const CommandQueue& ICommand::getCommandQueue() const noexcept { return *commandQueue; }

    const std::vector<ICommand*>& ICommand::getDependencies() const noexcept { return dependencies; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void ICommand::setName(std::string value) { name = std::move(value); }

    void ICommand::addDependency(ICommand& command)
    {
        if (command.commandQueue != commandQueue)
            throw SolError("Cannot add dependency, commands are not in same queue.");
        dependencies.push_back(&command);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string ICommand::getVizShape() const { return "rect"; }

    std::string ICommand::getVizFillColor() const { return "white"; }

    std::string ICommand::getVizOutlineColor() const { return "black"; }
}  // namespace sol
