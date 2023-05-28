#include "sol-core/version.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

namespace sol
{
    uint32_t Version::get() const noexcept { return VK_MAKE_API_VERSION(0, major, minor, patch); }

    uint32_t Version::getApiVersion() noexcept { return VK_API_VERSION_1_3; }

    uint32_t Version::getEngineVersion() noexcept
    {
        return VK_MAKE_API_VERSION(0, SOL_VERSION_MAJOR, SOL_VERSION_MINOR, SOL_VERSION_PATCH);
    }
}  // namespace sol
