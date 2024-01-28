#include "testutils/materials.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <fstream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_graphics_pipeline2.h"
#include "sol-core/vulkan_graphics_pipeline_fragment.h"
#include "sol-core/vulkan_graphics_pipeline_fragment_output.h"
#include "sol-core/vulkan_graphics_pipeline_pre_rasterization.h"
#include "sol-core/vulkan_graphics_pipeline_vertex_input.h"
#include "sol-core/vulkan_pipeline_layout.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-core/utils.h"
#include "sol-material/graphics/graphics_material2.h"

namespace
{
    [[nodiscard]] std::vector<std::byte> loadShaderBytecode(const std::string& name)
    {
        std::ifstream          file(name, std::ios::binary | std::ios::ate);
        const auto             size = file.tellg();
        std::vector<std::byte> code(size);
        file.seekg(std::ios::beg);
        file.read(reinterpret_cast<char*>(code.data()), size);
        return code;
    }

    [[nodiscard]] Materials::Graphics loadGraphicsSimple(sol::VulkanDevice& device)
    {
        std::vector<sol::DescriptorLayoutPtr> descriptorLayouts;

        descriptorLayouts.emplace_back(std::make_unique<sol::DescriptorLayout>(device));
        descriptorLayouts.back()->add(sol::DescriptorLayout::UniformBufferBinding{
          .binding = 0, .size = sizeof(float) * 16, .count = 1, .stages = VK_SHADER_STAGE_VERTEX_BIT});
        descriptorLayouts.back()->finalize();

        descriptorLayouts.emplace_back(std::make_unique<sol::DescriptorLayout>(device));
        descriptorLayouts.back()->add(
          sol::DescriptorLayout::SampledImageBinding{.binding = 0, .count = 1, .stages = VK_SHADER_STAGE_FRAGMENT_BIT});
        descriptorLayouts.back()->add(
          sol::DescriptorLayout::SamplerBinding{.binding = 1, .count = 1, .stages = VK_SHADER_STAGE_FRAGMENT_BIT});
        descriptorLayouts.back()->finalize();

        sol::VulkanPipelineLayout::Settings layoutSettings;
        layoutSettings.device = device;
        layoutSettings.descriptors += descriptorLayouts[0]->getLayout();
        layoutSettings.descriptors += descriptorLayouts[1]->getLayout();
        auto layout = sol::VulkanPipelineLayout::create(layoutSettings);

        sol::VulkanGraphicsPipelineVertexInput::Settings vertexInputSettings;
        vertexInputSettings.device = device;
        vertexInputSettings.vertexAttributes.emplace_back(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
        vertexInputSettings.vertexAttributes.emplace_back(1, 0, VK_FORMAT_R32G32_SFLOAT, 12);
        vertexInputSettings.vertexBindings.emplace_back(0, 20, VK_VERTEX_INPUT_RATE_VERTEX);


        sol::VulkanGraphicsPipelinePreRasterization::Settings preRastSettings;
        preRastSettings.layout            = layout;
        preRastSettings.vertexShader.code = loadShaderBytecode("simple_vs.spv");
        preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);
        preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);


        sol::VulkanGraphicsPipelineFragment::Settings fragmentSettings;
        fragmentSettings.layout              = layout;
        fragmentSettings.fragmentShader.code = loadShaderBytecode("simple_ps.spv");


        sol::VulkanGraphicsPipelineFragmentOutput::Settings fragOutSettings;
        fragOutSettings.device = device;
        fragOutSettings.colorBlend.attachments.emplace_back(VK_FALSE);
        fragOutSettings.colorAttachmentFormats.push_back(VK_FORMAT_R8G8B8A8_SRGB);
        fragOutSettings.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

        // graphics_pipeline_library is not enabled by default, so creating complete pipeline for now.
#if 0
        auto preRastPipeline = sol::VulkanGraphicsPipelinePreRasterization::create(preRastSettings);
        auto vertexInputPipeline = sol::VulkanGraphicsPipelineVertexInput::create(vertexInputSettings);
        auto fragPipeline = sol::VulkanGraphicsPipelineFragment::create(fragmentSettings);
        auto fragOutPipeline = sol::VulkanGraphicsPipelineFragmentOutput::create(fragOutSettings);

        sol::VulkanGraphicsPipeline2::Settings pipelineSettings;
        pipelineSettings.device = device;
        pipelineSettings.vertexInputPipeline = vertexInputPipeline;
        pipelineSettings.preRasterizationPipeline = preRastPipeline;
        pipelineSettings.fragmentPipeline = fragPipeline;
        pipelineSettings.fragmentOutputPipeline = fragOutPipeline;
        auto pipeline = sol::VulkanGraphicsPipeline2::create(pipelineSettings);
#else
        sol::VulkanGraphicsPipeline2::Settings2 pipelineSettings;
        pipelineSettings.vertexInput      = vertexInputSettings;
        pipelineSettings.preRasterization = preRastSettings;
        pipelineSettings.fragment         = fragmentSettings;
        pipelineSettings.fragmentOutput   = fragOutSettings;
        auto pipeline                     = sol::VulkanGraphicsPipeline2::create2(pipelineSettings);
#endif

        auto material = std::make_unique<sol::GraphicsMaterial2>(std::move(pipeline), raw(descriptorLayouts));
        return Materials::Graphics{.descriptorLayouts = std::move(descriptorLayouts), .material = std::move(material)};
    }
}  // namespace

Materials::Graphics Materials::load(const Graphics::Name name, sol::VulkanDevice& device)
{
    switch (name)
    {
    case Graphics::Name::Simple: return loadGraphicsSimple(device);
    }

    throw std::runtime_error("");
}
