#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class ForwardMaterialManager;
    class ForwardRenderData;
    class ForwardRenderer;
    class ForwardTraverser;

    using ForwardMaterialManagerPtr       = std::unique_ptr<ForwardMaterialManager>;
    using ForwardMaterialManagerSharedPtr = std::shared_ptr<ForwardMaterialManager>;
    using ForwardRenderDataPtr            = std::unique_ptr<ForwardRenderData>;
    using ForwardRenderDataSharedPtr      = std::shared_ptr<ForwardRenderData>;
    using ForwardRendererPtr              = std::unique_ptr<ForwardRenderer>;
    using ForwardRendererSharedPtr        = std::shared_ptr<ForwardRenderer>;
    using ForwardTraverserPtr             = std::unique_ptr<ForwardTraverser>;
    using ForwardTraverserSharedPtr       = std::shared_ptr<ForwardTraverser>;
}  // namespace sol