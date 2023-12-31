#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class GraphicsDynamicStateNode;
    class GraphicsMaterialNode;
    class GraphicsPushConstantNode;
    class MeshNode;
    class Node;
    class Scenegraph;

    using GraphicsDynamicStateNodePtr       = std::unique_ptr<GraphicsDynamicStateNode>;
    using GraphicsDynamicStateNodeSharedPtr = std::shared_ptr<GraphicsDynamicStateNode>;
    using GraphicsMaterialNodePtr           = std::unique_ptr<GraphicsMaterialNode>;
    using GraphicsMaterialNodeSharedPtr     = std::shared_ptr<GraphicsMaterialNode>;
    using GraphicsPushConstantNodePtr       = std::unique_ptr<GraphicsPushConstantNode>;
    using GraphicsPushConstantNodeSharedPtr = std::shared_ptr<GraphicsPushConstantNode>;
    using MeshNodePtr                       = std::unique_ptr<MeshNode>;
    using MeshNodeSharedPtr                 = std::shared_ptr<MeshNode>;
    using NodePtr                           = std::unique_ptr<Node>;
    using NodeSharedPtr                     = std::shared_ptr<Node>;
    using ScenegraphPtr                     = std::unique_ptr<Scenegraph>;
    using ScenegraphSharedPtr               = std::shared_ptr<Scenegraph>;
}  // namespace sol