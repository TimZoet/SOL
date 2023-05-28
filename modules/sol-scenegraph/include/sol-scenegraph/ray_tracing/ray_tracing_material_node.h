#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/ray_tracing/ray_tracing_material_instance.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class RayTracingMaterialNode final : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingMaterialNode() = default;

        explicit RayTracingMaterialNode(RayTracingMaterialInstance& materialInstance);

        RayTracingMaterialNode(const RayTracingMaterialNode&) = delete;

        RayTracingMaterialNode(RayTracingMaterialNode&&) = delete;

        ~RayTracingMaterialNode() noexcept override = default;

        RayTracingMaterialNode& operator=(const RayTracingMaterialNode&) = delete;

        RayTracingMaterialNode& operator=(RayTracingMaterialNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] RayTracingMaterialInstance* getMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterial(RayTracingMaterialInstance* mtl) noexcept;

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

        RayTracingMaterialInstance* material = nullptr;
    };
}  // namespace sol