#include "sol-error/vulkan_no_devices_error.h"

namespace sol
{
    VulkanNoDevicesError::VulkanNoDevicesError(const std::string& msg, std::source_location loc) :
        VulkanError(msg, std::move(loc))
    {
    }
}  // namespace sol
