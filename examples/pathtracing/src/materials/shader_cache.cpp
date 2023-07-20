#include "pathtracing/materials/shader_cache.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"

sol::ShaderCachePtr loadShaderCache(const std::filesystem::path& file)
{
    auto [cache, created] = sol::ShaderCache::openOrCreate(file);

    const auto importAndCompile = [&cache](const std::string&                        shaderFile,
                                           std::string                               identifier,
                                           const sol::VulkanShaderModule::ShaderType shaderType) {
        const auto source =
          cache->importSource(shaderFile, std::move(identifier), shaderType, sol::ShaderCache::OverwriteAction::Throw);

        static_cast<void>(source->compile(source->getIdentifier(), false));
    };

    if (created)
    {
        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\gui.vert)",
                         "vertex/gui",
                         sol::VulkanShaderModule::ShaderType::Vertex);

        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\gui.frag)",
                         "fragment/gui",
                         sol::VulkanShaderModule::ShaderType::Fragment);

        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\viewer.vert)",
                         "vertex/viewer",
                         sol::VulkanShaderModule::ShaderType::Vertex);

        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\viewer_attributes.frag)",
                         "fragment/viewer_attributes",
                         sol::VulkanShaderModule::ShaderType::Fragment);

        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\viewer_lit.frag)",
                         "fragment/viewer_lit",
                         sol::VulkanShaderModule::ShaderType::Fragment);

        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\viewer_textures.frag)",
                         "fragment/viewer_textures",
                         sol::VulkanShaderModule::ShaderType::Fragment);

        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\display.vert)",
                         "vertex/display",
                         sol::VulkanShaderModule::ShaderType::Vertex);

        importAndCompile(R"(C:\\Users\\timzo\\dev\\projects\\SOL\\source\\shaders\\sol_viewer\\display.frag)",
                         "fragment/display",
                         sol::VulkanShaderModule::ShaderType::Fragment);
    }

    return std::move(cache);
}
