#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class GraphicsRenderData;
    class GraphicsTraverser;

    using GraphicsRenderDataPtr       = std::unique_ptr<GraphicsRenderData>;
    using GraphicsRenderDataSharedPtr = std::shared_ptr<GraphicsRenderData>;
    using GraphicsTraverserPtr        = std::unique_ptr<GraphicsTraverser>;
    using GraphicsTraverserSharedPtr  = std::shared_ptr<GraphicsTraverser>;
}  // namespace sol
