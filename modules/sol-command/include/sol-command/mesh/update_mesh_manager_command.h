#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class UpdateMeshManagerCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Action
        {
            Transfer = 1,
            Deallocate = 2,
            All = 3
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateMeshManagerCommand();

        UpdateMeshManagerCommand(const UpdateMeshManagerCommand&) = delete;

        UpdateMeshManagerCommand(UpdateMeshManagerCommand&&) = delete;

        ~UpdateMeshManagerCommand() noexcept override;

        UpdateMeshManagerCommand& operator=(const UpdateMeshManagerCommand&) = delete;

        UpdateMeshManagerCommand& operator=(UpdateMeshManagerCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMeshManager(MeshManager& manager);

        void setAction(Action a) noexcept;

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

        MeshManager* meshManager = nullptr;

        Action action = Action::All;
    };
}  // namespace sol