#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    class VulkanError : public SolError
    {
    public:
        explicit VulkanError(const std::string& msg, std::source_location loc = std::source_location::current());
    };
}  // namespace sol