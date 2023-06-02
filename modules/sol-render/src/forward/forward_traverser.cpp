#include "sol-render/forward/forward_traverser.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/forward/forward_material_node.h"
#include "sol-scenegraph/forward/forward_push_constant_node.h"
#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/node.h"
#include "sol-scenegraph/scenegraph.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/forward/forward_render_data.h"

namespace
{
    constexpr size_t no_parent = static_cast<size_t>(-1);

    struct MaterialItem
    {
        /**
         * \brief Material instance.
         */
        const sol::ForwardMaterialInstance& material;

        /**
         * \brief Index of parent material.
         */
        const size_t parentIndex;
    };

    struct MeshItem
    {
        /**
         * \brief Mesh instance.
         */
        const sol::IMesh& mesh;

        /**
         * \brief Index of last visited material.
         */
        const size_t materialIndex;

        /**
         * \brief Index of last visited push constant.
         */
        const size_t pushConstantIndex;
    };

    struct PushConstantItem
    {
        /**
         * \brief Material.
         */
        const sol::ForwardMaterial& material;

        const uint32_t rangeOffset;

        const uint32_t rangeSize;

        const VkShaderStageFlags stages;

        /**
         * \brief Offset in push constant data array.
         */
        const size_t offset;

        /**
         * \brief Index of parent push constant.
         */
        const size_t parentIndex;
    };

    struct StackLists
    {
        std::vector<MaterialItem>     materials;
        std::vector<MeshItem>         meshes;
        std::vector<PushConstantItem> pushConstants;
        std::vector<std::byte>        pushConstantData;
    };

    struct StackItem
    {
        /**
         * \brief Node.
         */
        const sol::Node& node;

        /**
         * \brief Index of last visited material.
         */
        const size_t materialIndex;

        /**
         * \brief Index of last visited push constant.
         */
        const size_t pushConstantIndex;
    };

    StackLists createStackLists(const sol::ForwardTraverser& traverser, const sol::Scenegraph& scenegraph)
    {
        std::vector<MaterialItem>     materialStack;
        std::vector<MeshItem>         meshStack;
        std::vector<PushConstantItem> pushConstantStack;
        std::vector<std::byte>        pushConstantData;

        // Add root node to stack.
        std::vector<StackItem> stack;
        stack.emplace_back(scenegraph.getRootNode(), static_cast<size_t>(-1), static_cast<size_t>(-1));

        // Traverse until all nodes have been visited.
        while (!stack.empty())
        {
            // Get current node.
            const auto current = stack.back();
            stack.pop_back();

            const auto nodeType      = current.node.getType();
            bool       visitNode     = true;
            bool       visitChildren = true;

            // Use general mask to determine whether to visit node and children.
            switch (traverser.getGeneralMaskFunction()(current.node.getGeneralMask()))
            {
            case sol::ITraverser::TraversalAction::Continue: break;
            case sol::ITraverser::TraversalAction::Terminate:
                visitNode     = false;
                visitChildren = false;
                break;
            case sol::ITraverser::TraversalAction::IgnoreChildren: visitChildren = false; break;
            case sol::ITraverser::TraversalAction::Skip: visitNode = false;
            }

            // Use type mask to determine whether to visit node and children.
            if (traverser.supportsNodeType(nodeType))
            {
                switch (traverser.getTypeMaskFunction()(current.node.getTypeMask()))
                {
                case sol::ITraverser::TraversalAction::Continue: break;
                case sol::ITraverser::TraversalAction::Terminate:
                    visitNode     = false;
                    visitChildren = false;
                    break;
                case sol::ITraverser::TraversalAction::IgnoreChildren: visitChildren = false; break;
                case sol::ITraverser::TraversalAction::Skip: visitNode = false;
                }
            }
            // Skip node if unsupported type.
            else
                visitNode = false;

            // By default pass on material index of last visited material.
            size_t materialIndex           = current.materialIndex;
            size_t parentPushConstantIndex = current.pushConstantIndex;

            if (visitNode)
            {
                if (nodeType == sol::Node::Type::ForwardMaterial)
                {
                    // Adding a new material, so pass on the index of that one.
                    materialIndex = materialStack.size();

                    // Add material to list.
                    const auto& materialNode = static_cast<const sol::ForwardMaterialNode&>(current.node);
                    const auto* material     = materialNode.getMaterial();
                    if (material) materialStack.emplace_back(*material, current.materialIndex);
                }
                else if (nodeType == sol::Node::Type::ForwardPushConstant)
                {
                    parentPushConstantIndex = current.pushConstantIndex;

                    // Add push constant range to list.
                    const auto& pushConstantNode = static_cast<const sol::ForwardPushConstantNode&>(current.node);
                    const auto& material         = pushConstantNode.getMaterial();
                    const auto [rangeOffset, rangeSize] = pushConstantNode.getRange();
                    const auto offset                   = pushConstantData.size();
                    pushConstantData.resize(pushConstantData.size() + rangeSize);
                    std::memcpy(pushConstantData.data() + offset, pushConstantNode.getData(), rangeSize);
                    pushConstantStack.emplace_back(material,
                                                   rangeOffset,
                                                   rangeSize,
                                                   pushConstantNode.getStageFlags(),
                                                   offset,
                                                   parentPushConstantIndex);
                    parentPushConstantIndex = pushConstantStack.size() - 1;
                }
                else if (nodeType == sol::Node::Type::Mesh)
                {
                    // Add mesh to list with last visited material.
                    const auto& meshNode = static_cast<const sol::MeshNode&>(current.node);
                    const auto* mesh     = meshNode.getMesh();
                    if (mesh) meshStack.emplace_back(*mesh, materialIndex, parentPushConstantIndex);
                }
            }

            if (visitChildren)
            {
                // Push children on stack with index to last visited material.
                for (const auto& child : current.node.getChildren() | std::views::reverse)
                    stack.emplace_back(*child, materialIndex, parentPushConstantIndex);
            }
        }

        return {.materials        = std::move(materialStack),
                .meshes           = std::move(meshStack),
                .pushConstants    = std::move(pushConstantStack),
                .pushConstantData = std::move(pushConstantData)};
    }

    [[nodiscard]] bool findMaterialInstances(const StackLists&                                 stack,
                                             const sol::ForwardMaterial&                       activeMtl,
                                             std::vector<const sol::ForwardMaterialInstance*>& mtlInstanceList,
                                             const MaterialItem*                               mtlItem)
    {
        const auto& activeMtlLayout = activeMtl.getLayout();

        mtlInstanceList.resize(activeMtlLayout.getSetCount());
        std::ranges::fill(mtlInstanceList, nullptr);
        size_t missingSets = mtlInstanceList.size();

        // Traverse upwards and look for a compatible material instance for each set.
        while (mtlItem)
        {
            const auto& currentMtlInstance = mtlItem->material;
            const auto  setIndex           = currentMtlInstance.getSetIndex();

            // If this material instance is for a not yet found set.
            if (!mtlInstanceList[setIndex])
            {
                bool        compatible = true;
                const auto& currentMtl = currentMtlInstance.getForwardMaterial();

                // If the current material is not the same as the active material, check if layouts are compatible.
                if (&currentMtl != &activeMtl)
                {
                    const auto& currentMtlLayout = currentMtl.getLayout();
                    // Check push constant compatibility.
                    if (!activeMtlLayout.getDescription().isPushConstantCompatible(currentMtlLayout.getDescription()))
                        compatible = false;

                    // Check if layout is compatible at least up to setIndex.
                    if (const auto compatibleSets = activeMtlLayout.getDescription().getDescriptorSetCompatibility(
                          currentMtlLayout.getDescription());
                        compatibleSets <= setIndex)
                        compatible = false;
                }

                //
                if (compatible)
                {
                    mtlInstanceList[setIndex] = &currentMtlInstance;
                    missingSets--;
                    if (missingSets == 0) break;
                }
            }

            // Get parent, if any.
            if (mtlItem->parentIndex != no_parent)
                mtlItem = &stack.materials[mtlItem->parentIndex];
            else
                mtlItem = nullptr;
        }

        return missingSets == 0;
    }

    void findPushConstants(const StackLists&           stack,
                           const sol::ForwardMaterial& activeMtl,
                           std::vector<size_t>&        pushConstantIndices,
                           size_t                      pcItemIndex)
    {
        const auto& activeMtlLayout = activeMtl.getLayout();

        // Traverse upwards and look for compatible push constants.
        const PushConstantItem* pcItem = &stack.pushConstants[pcItemIndex];
        while (pcItem)
        {
            const auto& currentMtl = pcItem->material;

            // If the current material is not the same as the active material, check if push constants are compatible.
            if (&currentMtl != &activeMtl)
            {
                const auto& currentMtlLayout = currentMtl.getLayout();
                if (!activeMtlLayout.getDescription().isPushConstantCompatible(currentMtlLayout.getDescription()))
                    continue;
            }

            pushConstantIndices.push_back(pcItemIndex);

            // Get parent, if any.
            if (pcItem->parentIndex != no_parent)
            {
                pcItemIndex = pcItem->parentIndex;
                pcItem      = &stack.pushConstants[pcItemIndex];
            }
            else
                pcItem = nullptr;
        }
    }
}  // namespace

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardTraverser::ForwardTraverser() = default;

    ForwardTraverser::~ForwardTraverser() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool ForwardTraverser::supportsNodeType(const Node::Type type) const noexcept
    {
        switch (type)
        {
        case Node::Type::Empty:
        case Node::Type::ForwardMaterial:
        case Node::Type::ForwardPushConstant:
        case Node::Type::Mesh: return true;
        default: return false;
        }
    }

    ////////////////////////////////////////////////////////////////
    // Traversal.
    ////////////////////////////////////////////////////////////////

    void ForwardTraverser::traverse(const Scenegraph& scenegraph, ForwardRenderData& renderData)
    {
        auto stack = createStackLists(*this, scenegraph);

        // TODO: Cache compatibility. Do that here, or in material(-manager)? Materials can be in different managers, so that complicates things.
        // std::unordered_map<std::pair<const ForwardMaterial*, const ForwardMaterial*>, std::pair<bool, uint32_t>>
        //   layoutCompatibility;

        // TODO: Use std::array with fixed size instead? Would need to limit max depth to something sensible.
        std::vector<const ForwardMaterialInstance*> mtlInstanceList;

        for (const auto& [mesh, materialIndex, pushConstantIndex] : stack.meshes)
        {
            // Skip if no material was found above the mesh.
            if (materialIndex == no_parent) continue;

            const auto* mtlItem = &stack.materials[materialIndex];

            // Get the active material, i.e. the first material above the mesh node.
            const auto& activeMtlInstance = mtlItem->material;
            const auto& activeMtl         = activeMtlInstance.getForwardMaterial();
            const auto& activeMtlLayout   = activeMtl.getLayout();

            // Skip if push constants are needed but none were found above the mesh.
            if (activeMtlLayout.getPushConstantCount() > 0 && pushConstantIndex == no_parent) continue;

            // Look for a material instance for each descriptor set in the material layout.
            if (!findMaterialInstances(stack, activeMtl, mtlInstanceList, mtlItem)) continue;

            // Add to RenderData.
            const size_t materialOffset = renderData.materialInstances.size();
            for (const auto* mtlInstance : mtlInstanceList) renderData.materialInstances.emplace_back(mtlInstance);

            size_t              pushConstantOffset = renderData.pushConstantRanges.size();
            std::vector<size_t> pushConstantIndices;

            // Look for each required push constant.
            if (pushConstantIndex != no_parent)
            {
                findPushConstants(stack, activeMtl, pushConstantIndices, pushConstantIndex);

                // Add to RenderData.
                for (const auto index : pushConstantIndices)
                {
                    renderData.pushConstantRanges.emplace_back(
                      ForwardRenderData::PushConstantRange{.rangeOffset = stack.pushConstants[index].rangeOffset,
                                                           .rangeSize   = stack.pushConstants[index].rangeSize,
                                                           .offset      = stack.pushConstants[index].offset,
                                                           .stages      = stack.pushConstants[index].stages});
                }
            }

            //
            renderData.drawables.emplace_back(
              ForwardRenderData::Drawable{.mesh               = &mesh,
                                          .material           = &activeMtl,
                                          .materialOffset     = materialOffset,
                                          .pushConstantOffset = pushConstantOffset,
                                          .pushConstantCount  = pushConstantIndices.size()});
        }

        // Move all push constant data to RenderData.
        renderData.pushConstantData = std::move(stack.pushConstantData);

        renderData.sortDrawablesByLayer();
    }
}  // namespace sol
