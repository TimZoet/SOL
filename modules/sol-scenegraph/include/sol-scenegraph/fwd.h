#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class DeferredMaterialNode;
    class GraphicsMaterialNode;
    class MeshNode;
    class Node;
    class Scenegraph;

    using DeferredMaterialNodePtr       = std::unique_ptr<DeferredMaterialNode>;
    using DeferredMaterialNodeSharedPtr = std::shared_ptr<DeferredMaterialNode>;
    using GraphicsMaterialNodePtr       = std::unique_ptr<GraphicsMaterialNode>;
    using GraphicsMaterialNodeSharedPtr = std::shared_ptr<GraphicsMaterialNode>;
    using MeshNodePtr                   = std::unique_ptr<MeshNode>;
    using MeshNodeSharedPtr             = std::shared_ptr<MeshNode>;
    using NodePtr                       = std::unique_ptr<Node>;
    using NodeSharedPtr                 = std::shared_ptr<Node>;
    using ScenegraphPtr                 = std::unique_ptr<Scenegraph>;
    using ScenegraphSharedPtr           = std::shared_ptr<Scenegraph>;
}  // namespace sol