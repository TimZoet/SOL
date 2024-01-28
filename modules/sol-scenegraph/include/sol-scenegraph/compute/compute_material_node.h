#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/compute/compute_material_instance.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class ComputeMaterialNode final : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeMaterialNode() = default;

        explicit ComputeMaterialNode(ComputeMaterialInstance& materialInstance);

        ComputeMaterialNode(const ComputeMaterialNode&) = delete;

        ComputeMaterialNode(ComputeMaterialNode&&) = delete;

        ~ComputeMaterialNode() noexcept override = default;

        ComputeMaterialNode& operator=(const ComputeMaterialNode&) = delete;

        ComputeMaterialNode& operator=(ComputeMaterialNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] ComputeMaterialInstance* getMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterial(ComputeMaterialInstance* mtl) noexcept;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ComputeMaterialInstance* material = nullptr;
    };
}  // namespace sol