#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error.h"

namespace sol
{
    class VulkanMissingValidationLayerError final : public VulkanError
    {
    public:
        explicit VulkanMissingValidationLayerError(const std::string&   msg,
                                                   std::source_location loc = std::source_location::current());
    };
}  // namespace sol
