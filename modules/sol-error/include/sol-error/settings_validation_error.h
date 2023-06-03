#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    class SettingsValidationError : public SolError
    {
    public:
        explicit SettingsValidationError(const std::string&   msg,
                                         std::source_location loc = std::source_location::current());
    };
}  // namespace sol
