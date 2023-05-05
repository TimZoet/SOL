#include "sol-error/vulkan_error.h"

namespace sol
{
    VulkanError::VulkanError(const std::string& msg, std::source_location loc) : SolError(msg, std::move(loc)) {}
}  // namespace sol