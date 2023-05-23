#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-render/compute/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class UpdateComputeMaterialManagerCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateComputeMaterialManagerCommand();

        UpdateComputeMaterialManagerCommand(const UpdateComputeMaterialManagerCommand&) = delete;

        UpdateComputeMaterialManagerCommand(UpdateComputeMaterialManagerCommand&&) = delete;

        ~UpdateComputeMaterialManagerCommand() noexcept override;

        UpdateComputeMaterialManagerCommand& operator=(const UpdateComputeMaterialManagerCommand&) = delete;

        UpdateComputeMaterialManagerCommand& operator=(UpdateComputeMaterialManagerCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] ComputeMaterialManager* getMaterialManager() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(ComputeMaterialManager& manager);

        void setFrameIndexPtr(uint32_t* ptr);

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

        // TODO: This should become the IComputeMaterialManager class.
        ComputeMaterialManager* materialManager = nullptr;

        uint32_t* frameIndexPtr = nullptr;
    };
}  // namespace sol