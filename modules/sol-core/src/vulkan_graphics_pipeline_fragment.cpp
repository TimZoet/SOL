#include "sol-core/vulkan_graphics_pipeline_fragment.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_pipeline_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelineFragment::VulkanGraphicsPipelineFragment(Settings set, const VkPipeline vkPipeline) :
        settings(std::move(set)), pipeline(vkPipeline)
    {
    }

    VulkanGraphicsPipelineFragment::~VulkanGraphicsPipelineFragment() noexcept
    {
        vkDestroyPipeline(getDevice().get(), pipeline, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanGraphicsPipelineFragmentPtr VulkanGraphicsPipelineFragment::create(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_unique<VulkanGraphicsPipelineFragment>(settings, pipeline);
    }

    VulkanGraphicsPipelineFragmentSharedPtr VulkanGraphicsPipelineFragment::createShared(const Settings& settings)
    {
        auto pipeline = createImpl(settings);
        return std::make_shared<VulkanGraphicsPipelineFragment>(settings, pipeline);
    }

    VkPipeline VulkanGraphicsPipelineFragment::createImpl(const Settings& settings)
    {
        VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{};
        libraryInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
        libraryInfo.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT;
        constexpr auto flags =
          VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;

        std::vector<VkShaderModuleCreateInfo>        shaderModules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        // Reserving for stable pointers.
        shaderModules.reserve(1);

        const auto addStage = [&shaderModules,
                               &shaderStages](const VulkanGraphicsPipelinePreRasterization::Settings::Shader& shader,
                                              const VkShaderStageFlagBits                                     flag) {
            auto& module    = shaderModules.emplace_back();
            module.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            module.flags    = shader.moduleFlags;
            module.codeSize = shader.code.size();
            module.pCode    = reinterpret_cast<const uint32_t*>(shader.code.data());

            auto& stage = shaderStages.emplace_back();
            stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage.pNext = &module;
            stage.flags = shader.stageFlags;
            stage.stage = flag;
            stage.pName = shader.entrypoint.c_str();
        };

        if (!settings.fragmentShader.code.empty()) addStage(settings.fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT);

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        depthStencilInfo.pNext                 = nullptr;
        depthStencilInfo.flags                 = settings.depthStencil.flags;
        depthStencilInfo.depthTestEnable       = settings.depthStencil.depthTestEnable;
        depthStencilInfo.depthWriteEnable      = settings.depthStencil.depthWriteEnable;
        depthStencilInfo.depthCompareOp        = settings.depthStencil.depthCompareOp;
        depthStencilInfo.depthBoundsTestEnable = settings.depthStencil.depthBoundsTestEnable;
        depthStencilInfo.stencilTestEnable     = settings.depthStencil.stencilTestEnable;
        depthStencilInfo.front                 = settings.depthStencil.front;
        depthStencilInfo.back                  = settings.depthStencil.back;
        depthStencilInfo.minDepthBounds        = settings.depthStencil.minDepthBounds;
        depthStencilInfo.maxDepthBounds        = settings.depthStencil.maxDepthBounds;

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext             = VK_NULL_HANDLE;
        dynamicStateInfo.flags             = 0;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(settings.enabledDynamicStates.size());
        dynamicStateInfo.pDynamicStates    = settings.enabledDynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType              = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext              = &libraryInfo;
        pipelineInfo.flags              = flags;
        pipelineInfo.stageCount         = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages            = shaderStages.data();
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.pDynamicState      = &dynamicStateInfo;
        pipelineInfo.layout             = settings.layout;

        // Create pipeline.
        VkPipeline pipeline;
        handleVulkanError(vkCreateGraphicsPipelines(
          settings.layout().getDevice().get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

        return pipeline;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanGraphicsPipelineFragment::Settings& VulkanGraphicsPipelineFragment::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanGraphicsPipelineFragment::getDevice() noexcept { return settings.layout().getDevice(); }

    const VulkanDevice& VulkanGraphicsPipelineFragment::getDevice() const noexcept
    {
        return settings.layout().getDevice();
    }

    const VkPipeline& VulkanGraphicsPipelineFragment::get() const noexcept { return pipeline; }

    const std::vector<VkDynamicState>& VulkanGraphicsPipelineFragment::getDynamicStates() const noexcept
    {
        return settings.enabledDynamicStates;
    }

}  // namespace sol