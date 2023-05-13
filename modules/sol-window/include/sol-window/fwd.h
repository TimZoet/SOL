#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class Window;

    using WindowPtr       = std::unique_ptr<Window>;
    using WindowSharedPtr = std::shared_ptr<Window>;
}  // namespace sol
