#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

namespace sol
{
    SolError::SolError(const std::string& msg, const std::source_location loc)
    {
        message = std::format("\"{}\" in {} at {}:{}", msg, loc.file_name(), loc.line(), loc.column());
    }

    const char* SolError::what() const noexcept { return message.c_str(); }
}  // namespace sol