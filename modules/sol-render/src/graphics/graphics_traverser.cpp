#include "sol-render/graphics/graphics_traverser.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-material/graphics/graphics_dynamic_state.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/graphics/graphics_dynamic_state_node.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/graphics/graphics_render_data.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsTraverser::GraphicsTraverser() = default;

    GraphicsTraverser::~GraphicsTraverser() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderData* GraphicsTraverser::getRenderData() const noexcept { return renderData; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsTraverser::setRenderData(GraphicsRenderData* data) noexcept { renderData = data; }

    ////////////////////////////////////////////////////////////////
    // Traversal.
    ////////////////////////////////////////////////////////////////

    void GraphicsTraverser::traverseBegin()
    {
        if (!renderData) throw SolError("Cannot begin traversal. No render data assigned.");
    }

    void GraphicsTraverser::traverseEnd() {}

    void GraphicsTraverser::visit(const Node& node, const Node*)
    {
        if (node.supportsType(Node::Type::GraphicsDynamicState))
            visitNode(*static_cast<const GraphicsDynamicStateNode*>(node.getAs(Node::Type::GraphicsDynamicState)));

        if (node.supportsType(Node::Type::GraphicsMaterial))
            visitNode(*static_cast<const GraphicsMaterialNode*>(node.getAs(Node::Type::GraphicsMaterial)));

        if (node.supportsType(Node::Type::GraphicsPushConstant))
            visitNode(*static_cast<const GraphicsPushConstantNode*>(node.getAs(Node::Type::GraphicsPushConstant)));

        if (node.supportsType(Node::Type::Mesh)) visitNode(*static_cast<const MeshNode*>(node.getAs(Node::Type::Mesh)));
    }

    ITraverser2::TraversalAction GraphicsTraverser::generalMask(const uint64_t) { return TraversalAction::Visit; }

    ITraverser2::TraversalAction GraphicsTraverser::typeMask(const uint64_t) { return TraversalAction::Visit; }

    void GraphicsTraverser::visitNode(const GraphicsDynamicStateNode& node)
    {
        if (!node.getStates().empty())
        {
            dynamicStateStack.push(node, renderData->dynamicStates.size());
            for (const auto& s : node.getStates()) renderData->dynamicStates.emplace_back(s->clone());
        }
    }

    void GraphicsTraverser::visitNode(const GraphicsMaterialNode& node)
    {
        if (node.getMaterial()) materialStack.push(node);
    }

    void GraphicsTraverser::visitNode(const GraphicsPushConstantNode& node)
    {
        if (node.getMaterial() && node.getData())
        {
            // Copy push constant data.
            const auto size   = node.getMaterial()->getPushConstantRanges()[node.getRangeIndex()].size;
            const auto offset = renderData->pushConstantData.size();
            renderData->pushConstantData.resize(offset + size);
            std::memcpy(renderData->pushConstantData.data() + offset, node.getData(), size);
            pushConstantStack.push(node, offset);
        }
    }

    void GraphicsTraverser::visitNode(const MeshNode& node)
    {
        if (!node.getMesh()) return;

        const auto* mtl = materialStack.getActive(node);
        if (!mtl) return;

        const auto& material = mtl->node->getMaterial()->getGraphicsMaterial();

        // Reserve space for all required descriptors.
        const size_t descriptorOffset = renderData->descriptors.size();
        const size_t descriptorCount  = material.getDescriptorLayouts().size();
        renderData->descriptors.resize(descriptorOffset + descriptorCount);

        // Traverse upwards to find all descriptors.
        size_t found = 0;
        while (found < descriptorCount)
        {
            const auto& materialNode     = *mtl->node;
            const auto& materialInstance = *materialNode.getMaterial();

            // Collect all descriptors in the instance that were not yet set by a descendant.
            if (&material == &materialInstance.getGraphicsMaterial())
            {
                for (const auto [desc, index] : materialInstance)
                {
                    if (!renderData->descriptors[descriptorOffset + index])
                    {
                        renderData->descriptors[descriptorOffset + index] = &desc;
                        found++;
                    }
                }
            }

            // Go to parent, if any.
            if (mtl->parent == -1) break;
            mtl = materialStack[mtl->parent];
        }

        // TODO: Create utility struct that does this in its destructor, here and for other resets in this method.
        // Failed to find all required descriptors. Reset descriptor list.
        if (found < descriptorCount)
        {
            renderData->descriptors.resize(descriptorOffset);
            return;
        }

        const auto* pc = pushConstantStack.getActive(node);

        // Reserve space for push constant ranges.
        const size_t pcOffset = renderData->pushConstantRanges.size();
        const size_t pcCount  = material.getPushConstantRanges().size();
        renderData->pushConstantRanges.resize(pcOffset + material.getPushConstantRanges().size());

        // Traverse upwards to find all push constant ranges.
        found = 0;
        while (pc && found < pcCount)
        {
            const auto& pushConstantNode = *pc->node;

            if (&material == pushConstantNode.getMaterial())
            {
                const auto index = pushConstantNode.getRangeIndex();
                if (renderData->pushConstantRanges[pcOffset + index].offset == ~0ULL)
                {
                    renderData->pushConstantRanges[pcOffset + index].offset = pc->data;
                    renderData->pushConstantRanges[pcOffset + index].stages = pushConstantNode.getStageFlags();
                    found++;
                }
            }

            // Go to parent, if any.
            if (pc->parent == -1) break;
            pc = pushConstantStack[pc->parent];
        }

        // Failed to find all required push constants. Reset lists.
        if (found < pcCount)
        {
            renderData->descriptors.resize(descriptorOffset);
            renderData->pushConstantRanges.resize(pcOffset);
            return;
        }

        const auto* dynState = dynamicStateStack.getActive(node);

        // Reserve space for dynamic states.
        const size_t dynStateOffset = renderData->dynamicStateReferences.size();
        const size_t dynStateCount  = material.getDynamicStates().size();
        renderData->dynamicStateReferences.resize(dynStateOffset + dynStateCount);

        // Traverse upwards to find all dynamic states.
        found = 0;
        while (dynState && found < dynStateCount)
        {
            const auto& dynStateNode = *dynState->node;

            // Dynamic states are stored in a random order. Go over all of the states for the current node and see if
            // 1. It is enabled for the material;
            // 2. It is not already set by a descendant;
            // 3. Not all states have been set yet.
            for (size_t i = 0; i < dynStateNode.getStates().size(); i++)
            {
                const auto& state      = *renderData->dynamicStates[dynState->data + i];
                bool        alreadySet = false;

                // Skip
                if (!material.isDynamicStateEnabled(state.getType())) continue;

                // Skip if it was already set by a descendant.
                for (size_t j = 0; j < found; j++)
                {
                    if (renderData->dynamicStateReferences[dynStateOffset + j]->getType() == state.getType())
                    {
                        alreadySet = true;
                        break;
                    }
                }
                if (alreadySet) continue;

                // Insert into next empty spot.
                renderData->dynamicStateReferences[dynStateOffset + found] = &state;
                found++;

                if (found == dynStateCount) break;
            }

            // Go to parent, if any.
            if (dynState->parent == -1) break;
            dynState = dynamicStateStack[dynState->parent];
        }

        // Failed to find all required dynamic states. Reset lists.
        if (found < dynStateCount)
        {
            renderData->descriptors.resize(descriptorOffset);
            renderData->pushConstantRanges.resize(pcOffset);
            renderData->dynamicStateReferences.resize(dynStateOffset);
            return;
        }

        renderData->drawables.emplace_back(GraphicsRenderData::Drawable{.mesh               = node.getMesh(),
                                                                        .material           = &material,
                                                                        .descriptorOffset   = descriptorOffset,
                                                                        .pushConstantOffset = pcOffset,
                                                                        .dynamicStateOffset = dynStateOffset});
    }
}  // namespace sol
