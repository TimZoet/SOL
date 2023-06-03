#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class GraphicsMaterialManager;
    class GraphicsPipelineCache;
    class GraphicsRenderData;
    class GraphicsRenderer;
    class GraphicsTraverser;

    using GraphicsMaterialManagerPtr       = std::unique_ptr<GraphicsMaterialManager>;
    using GraphicsMaterialManagerSharedPtr = std::shared_ptr<GraphicsMaterialManager>;
    using GraphicsPipelineCachePtr         = std::unique_ptr<GraphicsPipelineCache>;
    using GraphicsPipelineCacheSharedPtr   = std::shared_ptr<GraphicsPipelineCache>;
    using GraphicsRenderDataPtr            = std::unique_ptr<GraphicsRenderData>;
    using GraphicsRenderDataSharedPtr      = std::shared_ptr<GraphicsRenderData>;
    using GraphicsRendererPtr              = std::unique_ptr<GraphicsRenderer>;
    using GraphicsRendererSharedPtr        = std::shared_ptr<GraphicsRenderer>;
    using GraphicsTraverserPtr             = std::unique_ptr<GraphicsTraverser>;
    using GraphicsTraverserSharedPtr       = std::shared_ptr<GraphicsTraverser>;
}  // namespace sol
