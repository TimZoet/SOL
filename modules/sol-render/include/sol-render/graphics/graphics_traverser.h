#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/fwd.h"
#include "sol-scenegraph/node.h"
#include "sol-scenegraph/graphics/graphics_dynamic_state_node.h"
#include "sol-scenegraph/graphics/graphics_material_node.h"
#include "sol-scenegraph/graphics/graphics_push_constant_node.h"
#include "sol-scenegraph/traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/fwd.h"

namespace sol
{
    class GraphicsTraverser : public Traverser<Node::Type::GraphicsDynamicState,
                                               Node::Type::GraphicsMaterial,
                                               Node::Type::GraphicsPushConstant,
                                               Node::Type::Mesh>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsTraverser();

        GraphicsTraverser(const GraphicsTraverser&) = delete;

        GraphicsTraverser(GraphicsTraverser&&) = delete;

        ~GraphicsTraverser() noexcept override;

        GraphicsTraverser& operator=(const GraphicsTraverser&) = delete;

        GraphicsTraverser& operator=(GraphicsTraverser&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] GraphicsRenderData* getRenderData() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setRenderData(GraphicsRenderData* data) noexcept;

    protected:
        ////////////////////////////////////////////////////////////////
        // Traversal.
        ////////////////////////////////////////////////////////////////

        void traverseBegin() override;

        void traverseEnd() override;

        void visit(const Node& node, const Node* previous) override;

        TraversalAction generalMask(uint64_t mask) override;

        TraversalAction typeMask(uint64_t mask) override;

        void visitNode(const GraphicsDynamicStateNode& node);

        void visitNode(const GraphicsMaterialNode& node);

        void visitNode(const GraphicsPushConstantNode& node);

        void visitNode(const MeshNode& node);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        TraversalStack<GraphicsDynamicStateNode, size_t> dynamicStateStack{};
        TraversalStack<GraphicsMaterialNode>             materialStack{};
        TraversalStack<GraphicsPushConstantNode, size_t> pushConstantStack{};
        GraphicsRenderData*                              renderData = nullptr;
    };
}  // namespace sol
