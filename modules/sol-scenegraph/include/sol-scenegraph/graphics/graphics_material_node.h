#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_material_instance.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class GraphicsMaterialNode final : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialNode() = default;

        explicit GraphicsMaterialNode(GraphicsMaterialInstance& materialInstance);

        GraphicsMaterialNode(const GraphicsMaterialNode&) = delete;

        GraphicsMaterialNode(GraphicsMaterialNode&&) = delete;

        ~GraphicsMaterialNode() noexcept override = default;

        GraphicsMaterialNode& operator=(const GraphicsMaterialNode&) = delete;

        GraphicsMaterialNode& operator=(GraphicsMaterialNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] GraphicsMaterialInstance* getMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterial(GraphicsMaterialInstance* mtl) noexcept;

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string getVizLabel() const override;

        [[nodiscard]] std::string getVizShape() const override;

        [[nodiscard]] std::string getVizFillColor() const override;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialInstance* material = nullptr;
    };
}  // namespace sol