#include "sol-core/vulkan_shader_module.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <shaderc/shaderc.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"
#include "sol-error/vulkan_shader_compilation_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanShaderModule::VulkanShaderModule(SettingsPtr settingsPtr, const VkShaderModule vkShaderModule) :
        settings(std::move(settingsPtr)), shaderModule(vkShaderModule)
    {
    }

    VulkanShaderModule::~VulkanShaderModule() noexcept
    {
        vkDestroyShaderModule(settings->device, shaderModule, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanShaderModulePtr VulkanShaderModule::create(Settings settings)
    {
        auto module = createImpl(settings);
        return std::make_unique<VulkanShaderModule>(std::make_unique<Settings>(std::move(settings)), module);
    }

    VulkanShaderModuleSharedPtr VulkanShaderModule::createShared(Settings settings)
    {
        auto module = createImpl(settings);
        return std::make_shared<VulkanShaderModule>(std::make_unique<Settings>(std::move(settings)), module);
    }

    VkShaderModule VulkanShaderModule::createImpl(const Settings& settings)
    {
        // Prepare creation info.
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        VkShaderModule shaderModule;

        // Use source code to compile shader.
        if (!settings.source.empty())
        {
            // Create compilation options.
            shaderc::CompileOptions options;
            options.SetSourceLanguage(shaderc_source_language_glsl);

            // Determine shader kind.
            auto kind = shaderc_glsl_infer_from_source;
            switch (settings.type)
            {
            case ShaderType::Automatic: kind = shaderc_glsl_infer_from_source; break;
            case ShaderType::Compute: kind = shaderc_glsl_compute_shader; break;
            case ShaderType::Fragment: kind = shaderc_glsl_fragment_shader; break;
            case ShaderType::Vertex: kind = shaderc_glsl_vertex_shader; break;
            }

            // Compile using either compiler passed through settings or one time only compiler.
            shaderc::SpvCompilationResult result;
            if (settings.compiler)
                result = settings.compiler->CompileGlslToSpv(settings.source, kind, settings.filename.c_str(), options);
            else
            {
                const shaderc::Compiler compiler;
                result = compiler.CompileGlslToSpv(settings.source, kind, settings.filename.c_str(), options);
            }

            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                const auto msg = result.GetErrorMessage();
                throw VulkanShaderCompilationError(msg);
            }

            createInfo.codeSize = static_cast<size_t>(result.cend() - result.cbegin()) * 4;
            createInfo.pCode    = result.cbegin();

            // Create shader module.
            handleVulkanError(vkCreateShaderModule(settings.device, &createInfo, nullptr, &shaderModule));
        }
        // Use precompiled code.
        else if (!settings.binary.empty())
        {
            createInfo.codeSize = static_cast<size_t>(settings.binary.cend() - settings.binary.cbegin());
            createInfo.pCode    = reinterpret_cast<const uint32_t*>(settings.binary.data());

            // Create shader module.
            handleVulkanError(vkCreateShaderModule(settings.device, &createInfo, nullptr, &shaderModule));
        }
        else
        {
            throw VulkanShaderCompilationError(
              "Could not create shader module because both source and binary were empty.");
        }

        return shaderModule;
    }


    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanShaderModule::Settings& VulkanShaderModule::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanShaderModule::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanShaderModule::getDevice() const noexcept { return settings->device(); }

    const VkShaderModule& VulkanShaderModule::get() const noexcept { return shaderModule; }
}  // namespace sol