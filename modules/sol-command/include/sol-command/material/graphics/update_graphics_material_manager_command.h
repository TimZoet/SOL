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
    class UpdateGraphicsMaterialManagerCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateGraphicsMaterialManagerCommand();

        UpdateGraphicsMaterialManagerCommand(const UpdateGraphicsMaterialManagerCommand&) = delete;

        UpdateGraphicsMaterialManagerCommand(UpdateGraphicsMaterialManagerCommand&&) = delete;

        ~UpdateGraphicsMaterialManagerCommand() noexcept override;

        UpdateGraphicsMaterialManagerCommand& operator=(const UpdateGraphicsMaterialManagerCommand&) = delete;

        UpdateGraphicsMaterialManagerCommand& operator=(UpdateGraphicsMaterialManagerCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] IGraphicsMaterialManager* getMaterialManager() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IGraphicsMaterialManager& manager);

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

        IGraphicsMaterialManager* materialManager = nullptr;

        uint32_t* frameIndexPtr = nullptr;
    };
}  // namespace sol
