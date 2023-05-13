#include "sol-command/command_queue.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <functional>
#include <ranges>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    CommandQueue::CommandQueue() = default;

    CommandQueue::~CommandQueue() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool CommandQueue::isFinalized() const noexcept { return finalized; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void CommandQueue::finalize()
    {
        requireNonFinalized();
        if (commands.empty()) throw SolError("Cannot finalize CommandQueue: no commands.");

        std::unordered_map<const ICommand*, size_t> commandDepths;

        using func_t              = std::function<void(const ICommand&, size_t)>;
        const func_t resolveDepth = [&](const ICommand& command, size_t depth) {
            if (depth > commands.size()) throw SolError("Cannot finalize CommandQueue: cycle detected.");

            if (const auto it = commandDepths.find(&command); it != commandDepths.end())
            {
                if (depth > it->second)
                {
                    it->second = depth;
                    for (const auto* dep : command.getDependencies()) resolveDepth(*dep, depth + 1);
                }
            }
            else
            {
                commandDepths.try_emplace(&command, depth);
                for (const auto* dep : command.getDependencies()) resolveDepth(*dep, depth + 1);
            }
        };

        for (const auto& [command, wait, notify] : commands) resolveDepth(*command, 0);

        // Sort commands by reverse depth.
        std::ranges::sort(commands.begin(), commands.end(), [&commandDepths](const Command& lhs, const Command& rhs) {
            return commandDepths.at(lhs.command.get()) > commandDepths.at(rhs.command.get());
        });

        for (auto& [command, wait, notify] : commands)
        {
            wait = std::make_unique<std::barrier<>>(command->getDependencies().size() + 1);
        }

        for (auto& [command, wait, notify] : commands)
        {
            for (auto* dep : command->getDependencies())
            {
                const size_t j = std::distance(
                  commands.cbegin(), std::ranges::find_if(commands.cbegin(), commands.cend(), [dep](const Command& c) {
                      return c.command.get() == dep;
                  }));

                commands[j].notify.emplace_back(wait.get());
            }
        }

        // Finalize commands.
        for (const auto& [command, wait, notify] : commands) command->finalize();

        finalized = true;
    }

    void CommandQueue::requireFinalized() const
    {
        if (!finalized) throw SolError("CommandQueue was not yet finalized.");
    }

    void CommandQueue::requireNonFinalized() const
    {
        if (finalized) throw SolError("CommandQueue was already finalized.");
    }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void CommandQueue::start()
    {
        requireFinalized();
        if (nextCommand != static_cast<size_t>(-1)) throw SolError("CommandQueue was already started.");

        nextCommand = 0;
    }

    std::optional<std::function<void()>> CommandQueue::getNextCommand()
    {
        requireFinalized();

        // Get index of next command.
        const auto commandIndex = nextCommand.fetch_add(1);

        // No more commands, return none.
        if (commandIndex >= commands.size()) return {};

        // Return function to execute command.
        auto& cmd = commands[commandIndex];
        return [&cmd, this]() {
            // Wait for dependencies to complete.
            cmd.wait->arrive_and_wait();

            // Execute command.
            cmd.command->operator()();

            // Notify dependencies of completion.
            for (auto* b : cmd.notify) static_cast<void>(b->arrive());
        };
    }

    void CommandQueue::end()
    {
        requireFinalized();

        if (nextCommand < commands.size()) throw SolError("Not all commands in the CommandQueue were retrieved.");

        // TODO: Add another barrier that is notified by all commands and waited on here? That way this end method will guarantee everything has been completed, not just retrieved.
        // Perhaps make this option configurable.

        nextCommand = -1;
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    /*void CommandQueue::visualize(dot::Graph& graph) const
    {
        requireFinalized();

        std::unordered_map<const ICommand*, dot::Node*> nodes;

        for (const auto& cmd : commands)
        {
            auto& node = graph.createNode();
            node.setLabel(cmd.command->getVizLabel() + "\\n" + cmd.command->getName());
            node.setShape(cmd.command->getVizShape());
            node.attributes["style"] = "filled";
            node.attributes["fillcolor"] = cmd.command->getVizFillColor();
            node.attributes["color"] = cmd.command->getVizOutlineColor();

            for (const auto* dep : cmd.command->getDependencies())
            {
                graph.createEdge(*nodes.at(dep), node);
            }

            nodes.try_emplace(cmd.command.get(), &node);
        }
    }*/
}  // namespace sol
