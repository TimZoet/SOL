#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_material_instance2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class GraphicsMaterialNode : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialNode();

        explicit GraphicsMaterialNode(uuids::uuid id);

        explicit GraphicsMaterialNode(GraphicsMaterialInstance2& m);

        GraphicsMaterialNode(uuids::uuid id, GraphicsMaterialInstance2& m);

        GraphicsMaterialNode(const GraphicsMaterialNode&) = delete;

        GraphicsMaterialNode(GraphicsMaterialNode&&) = delete;

        ~GraphicsMaterialNode() noexcept override;

        GraphicsMaterialNode& operator=(const GraphicsMaterialNode&) = delete;

        GraphicsMaterialNode& operator=(GraphicsMaterialNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] GraphicsMaterialInstance2* getMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterial(GraphicsMaterialInstance2* mtl) noexcept;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialInstance2* material = nullptr;
    };
}  // namespace sol