#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

namespace sol
{
    std::vector<const char*> stringVectorToConstCharVector(const std::vector<std::string>& strings);
}  // namespace sol
