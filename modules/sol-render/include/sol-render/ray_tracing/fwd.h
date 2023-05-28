#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class RayTracingMaterialManager;
    class RayTracingPipelineCache;
    class RayTracingRenderData;
    class RayTracingRenderer;
    class RayTracingTraverser;

    using RayTracingMaterialManagerPtr       = std::unique_ptr<RayTracingMaterialManager>;
    using RayTracingMaterialManagerSharedPtr = std::shared_ptr<RayTracingMaterialManager>;
    using RayTracingPipelineCachePtr         = std::unique_ptr<RayTracingPipelineCache>;
    using RayTracingPipelineCacheSharedPtr   = std::shared_ptr<RayTracingPipelineCache>;
    using RayTracingRenderDataPtr            = std::unique_ptr<RayTracingRenderData>;
    using RayTracingRenderDataSharedPtr      = std::shared_ptr<RayTracingRenderData>;
    using RayTracingRendererPtr              = std::unique_ptr<RayTracingRenderer>;
    using RayTracingRendererSharedPtr        = std::shared_ptr<RayTracingRenderer>;
    using RayTracingTraverserPtr             = std::unique_ptr<RayTracingTraverser>;
    using RayTracingTraverserSharedPtr       = std::shared_ptr<RayTracingTraverser>;
}  // namespace sol
