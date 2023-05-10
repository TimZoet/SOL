#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class MemoryManager;

    using MemoryManagerPtr       = std::unique_ptr<MemoryManager>;
    using MemoryManagerSharedPtr = std::shared_ptr<MemoryManager>;
}  // namespace sol
