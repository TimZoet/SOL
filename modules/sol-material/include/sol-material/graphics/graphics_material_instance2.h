#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/material_instance2.h"

namespace sol
{
    class GraphicsMaterialInstance2 : public MaterialInstance2
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialInstance2() = delete;

        explicit GraphicsMaterialInstance2(GraphicsMaterial2& mtl);

        GraphicsMaterialInstance2(const GraphicsMaterialInstance2&) = delete;

        GraphicsMaterialInstance2(GraphicsMaterialInstance2&&) = delete;

        ~GraphicsMaterialInstance2() noexcept override;

        GraphicsMaterialInstance2& operator=(const GraphicsMaterialInstance2&) = delete;

        GraphicsMaterialInstance2& operator=(GraphicsMaterialInstance2&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] GraphicsMaterial2& getGraphicsMaterial() noexcept;

        [[nodiscard]] const GraphicsMaterial2& getGraphicsMaterial() const noexcept;
    };
}  // namespace sol
