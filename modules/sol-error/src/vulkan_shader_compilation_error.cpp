#include "sol-error/vulkan_shader_compilation_error.h"

namespace sol
{
    VulkanShaderCompilationError::VulkanShaderCompilationError(const std::string& msg, std::source_location loc) :
        VulkanError(msg, std::move(loc))
    {
    }
}  // namespace sol
