#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class ITraverser;
    class RenderSettings;
    class UniformBuffer;
    class UniformBufferManager;

    using ITraverserPtr                 = std::unique_ptr<ITraverser>;
    using ITraverserSharedPtr           = std::shared_ptr<ITraverser>;
    using RenderSettingsPtr             = std::unique_ptr<RenderSettings>;
    using RenderSettingsSharedPtr       = std::shared_ptr<RenderSettings>;
    using UniformBufferPtr              = std::unique_ptr<UniformBuffer>;
    using UniformBufferSharedPtr        = std::shared_ptr<UniformBuffer>;
    using UniformBufferManagerPtr       = std::unique_ptr<UniformBufferManager>;
    using UniformBufferManagerSharedPtr = std::shared_ptr<UniformBufferManager>;
}  // namespace sol