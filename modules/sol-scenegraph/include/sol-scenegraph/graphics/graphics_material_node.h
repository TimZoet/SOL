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
    class ForwardMaterialNode final : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardMaterialNode() = default;

        explicit ForwardMaterialNode(ForwardMaterialInstance& materialInstance);

        ForwardMaterialNode(const ForwardMaterialNode&) = delete;

        ForwardMaterialNode(ForwardMaterialNode&&) = delete;

        ~ForwardMaterialNode() noexcept override = default;

        ForwardMaterialNode& operator=(const ForwardMaterialNode&) = delete;

        ForwardMaterialNode& operator=(ForwardMaterialNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] ForwardMaterialInstance* getMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterial(ForwardMaterialInstance* mtl) noexcept;

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

        ForwardMaterialInstance* material = nullptr;
    };
}  // namespace sol