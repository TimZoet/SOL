#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class UpdateForwardMaterialManagerCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateForwardMaterialManagerCommand();

        UpdateForwardMaterialManagerCommand(const UpdateForwardMaterialManagerCommand&) = delete;

        UpdateForwardMaterialManagerCommand(UpdateForwardMaterialManagerCommand&&) = delete;

        ~UpdateForwardMaterialManagerCommand() noexcept override;

        UpdateForwardMaterialManagerCommand& operator=(const UpdateForwardMaterialManagerCommand&) = delete;

        UpdateForwardMaterialManagerCommand& operator=(UpdateForwardMaterialManagerCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] IForwardMaterialManager* getMaterialManager() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IForwardMaterialManager& manager);

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

        IForwardMaterialManager* materialManager = nullptr;

        uint32_t* frameIndexPtr = nullptr;
    };
}  // namespace sol
