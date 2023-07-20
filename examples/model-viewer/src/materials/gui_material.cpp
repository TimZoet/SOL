#include "pathtracing/materials/gui_material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"
#include "sol-mesh/mesh_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////


#include "pathtracing/materials/camera_material.h"
#include "pathtracing/materials/shader_cache.h"

GuiMaterial::GuiMaterial(sol::VulkanShaderModuleSharedPtr vertexModule,
                         sol::VulkanShaderModuleSharedPtr fragmentModule) :
    ForwardMaterial(std::move(vertexModule), std::move(fragmentModule))
{
    auto& pc  = layout.addPushConstant();
    pc.name   = "Transform";
    pc.offset = 0;
    pc.size   = sizeof(float) * 4;
    pc.stages = VK_SHADER_STAGE_VERTEX_BIT;

    auto& sampler   = layout.addSampler();
    sampler.name    = "Texture";
    sampler.set     = 0;
    sampler.binding = 0;
    sampler.count   = 1;
    sampler.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkPipelineColorBlendAttachmentState colorBlending;
    colorBlending.blendEnable         = VK_TRUE;
    colorBlending.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlending.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlending.colorBlendOp        = VK_BLEND_OP_ADD;
    colorBlending.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlending.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlending.alphaBlendOp        = VK_BLEND_OP_ADD;
    colorBlending.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    layout.addColorBlending(colorBlending);

    layout.finalize();
}

GuiMaterial::~GuiMaterial() noexcept = default;

GuiMaterialInstance::GuiMaterialInstance(sol::Texture2D& tex) : ForwardMaterialInstance(), fontTexture(&tex) {}

GuiMaterialInstance::~GuiMaterialInstance() noexcept = default;

std::unique_ptr<GuiMaterial> GuiMaterial::create(sol::VulkanDevice& device, sol::ShaderCache& shaderCache)
{
    const auto vertexShader   = shaderCache.getModule("vertex/imgui");
    const auto fragmentShader = shaderCache.getModule("fragment/imgui");
    auto       vertexModule   = vertexShader->createVulkanShaderModuleShared(device);
    auto       fragmentModule = fragmentShader->createVulkanShaderModuleShared(device);

    return std::make_unique<GuiMaterial>(std::move(vertexModule), std::move(fragmentModule));
}

////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////

uint32_t GuiMaterialInstance::getSetIndex() const { return 0; }

bool GuiMaterialInstance::isUniformBufferStale(size_t binding) const { return false; }

const void* GuiMaterialInstance::getUniformBufferData(size_t binding) const { return nullptr; }

sol::Texture2D* GuiMaterialInstance::getTextureData(size_t binding) const { return fontTexture; }

////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////

void GuiMaterialInstance::setFontTexture(sol::Texture2D* tex) { fontTexture = tex; }
