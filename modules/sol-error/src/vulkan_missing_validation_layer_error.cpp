#include "sol-error/vulkan_missing_validation_layer_error.h"

namespace sol
{
    VulkanMissingValidationLayerError::VulkanMissingValidationLayerError(const std::string&   msg,
                                                                         std::source_location loc) :
        VulkanError(msg, std::move(loc))
    {
    }
}  // namespace sol
