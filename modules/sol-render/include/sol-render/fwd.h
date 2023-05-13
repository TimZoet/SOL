#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class ITraverser;
    class RenderSettings;

    using ITraverserPtr           = std::unique_ptr<ITraverser>;
    using ITraverserSharedPtr     = std::shared_ptr<ITraverser>;
    using RenderSettingsPtr       = std::unique_ptr<RenderSettings>;
    using RenderSettingsSharedPtr = std::shared_ptr<RenderSettings>;
}  // namespace sol