#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error.h"

namespace sol
{
    class VulkanShaderCompilationError final : public VulkanError
    {
    public:
        explicit VulkanShaderCompilationError(const std::string&   msg,
                                              std::source_location loc = std::source_location::current());
    };
}  // namespace sol
