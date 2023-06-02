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
    class UpdateRayTracingMaterialManagerCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateRayTracingMaterialManagerCommand();

        UpdateRayTracingMaterialManagerCommand(const UpdateRayTracingMaterialManagerCommand&) = delete;

        UpdateRayTracingMaterialManagerCommand(UpdateRayTracingMaterialManagerCommand&&) = delete;

        ~UpdateRayTracingMaterialManagerCommand() noexcept override;

        UpdateRayTracingMaterialManagerCommand& operator=(const UpdateRayTracingMaterialManagerCommand&) = delete;

        UpdateRayTracingMaterialManagerCommand& operator=(UpdateRayTracingMaterialManagerCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] IRayTracingMaterialManager* getMaterialManager() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IRayTracingMaterialManager& manager);

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

        IRayTracingMaterialManager* materialManager = nullptr;

        uint32_t* frameIndexPtr = nullptr;
    };
}  // namespace sol