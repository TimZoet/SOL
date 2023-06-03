#include "sol-error/settings_validation_error.h"

namespace sol
{
    SettingsValidationError::SettingsValidationError(const std::string& msg, std::source_location loc) :
        SolError(msg, std::move(loc))
    {
    }
}  // namespace sol
