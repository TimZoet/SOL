#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

//#include "dot/graph.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/fwd.h"

namespace sol
{
    class ICommand
    {
        friend class CommandQueue;

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ICommand() = default;

        ICommand(const ICommand&) = delete;

        ICommand(ICommand&&) = delete;

        virtual ~ICommand() noexcept = default;

        ICommand& operator=(const ICommand&) = delete;

        ICommand& operator=(ICommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const std::string& getName() const noexcept;

        [[nodiscard]] CommandQueue& getCommandQueue() noexcept;

        [[nodiscard]] const CommandQueue& getCommandQueue() const noexcept;

        [[nodiscard]] const std::vector<ICommand*>& getDependencies() const noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void setName(std::string value);

        void addDependency(ICommand& command);

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        virtual void finalize() = 0;

        virtual void operator()() = 0;

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual std::string getVizLabel() const = 0;

        [[nodiscard]] virtual std::string getVizShape() const;

        [[nodiscard]] virtual std::string getVizFillColor() const;

        [[nodiscard]] virtual std::string getVizOutlineColor() const;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string name;

        CommandQueue* commandQueue = nullptr;

        std::vector<ICommand*> dependencies;
    };
}  // namespace sol