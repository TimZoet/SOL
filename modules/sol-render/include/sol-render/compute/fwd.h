#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class ComputeMaterialManager;
    class ComputePipelineCache;
    class ComputeRenderData;
    class ComputeRenderer;
    class ComputeTraverser;

    using ComputeMaterialManagerPtr       = std::unique_ptr<ComputeMaterialManager>;
    using ComputeMaterialManagerSharedPtr = std::shared_ptr<ComputeMaterialManager>;
    using ComputePipelineCachePtr         = std::unique_ptr<ComputePipelineCache>;
    using ComputePipelineCacheSharedPtr   = std::shared_ptr<ComputePipelineCache>;
    using ComputeRenderDataPtr            = std::unique_ptr<ComputeRenderData>;
    using ComputeRenderDataSharedPtr      = std::shared_ptr<ComputeRenderData>;
    using ComputeRendererPtr              = std::unique_ptr<ComputeRenderer>;
    using ComputeRendererSharedPtr        = std::shared_ptr<ComputeRenderer>;
    using ComputeTraverserPtr             = std::unique_ptr<ComputeTraverser>;
    using ComputeTraverserSharedPtr       = std::shared_ptr<ComputeTraverser>;
}  // namespace sol
