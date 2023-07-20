#include "model-viewer/materials/display_material.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

DisplayMaterial::DisplayMaterial(sol::MeshLayout&         mLayout,
                                 sol::VulkanShaderModule& vertexModule,
                                 sol::VulkanShaderModule& fragmentModule) :
    GraphicsMaterial(vertexModule, fragmentModule)
{
    layout.setMeshLayout(mLayout);

    layout.enableDynamicState<VK_DYNAMIC_STATE_VIEWPORT>(true);
    layout.enableDynamicState<VK_DYNAMIC_STATE_SCISSOR>(true);
    layout.setCullMode(sol::CullMode::None);
    layout.setDynamicViewportCount(1);
    layout.setDynamicScissorCount(1);

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
    layout.addColorBlendAttachment(colorBlending);

    sol::MaterialLayoutDescription desc;
    auto&                          sampler = desc.combinedImageSamplers.emplace_back();
    sampler.name                           = "Texture";
    sampler.set                            = 0;
    sampler.binding                        = 0;
    sampler.count                          = 1;
    sampler.stages                         = VK_SHADER_STAGE_FRAGMENT_BIT;

    layout.finalize(std::move(desc));
}

DisplayMaterialInstance::DisplayMaterialInstance(math::uint2 resolution, sol::Texture2D& tex) : texture(&tex)
{
    std::vector<VkViewport> viewports;
    viewports.emplace_back(0.0f, 0.0f, static_cast<float>(resolution.x), static_cast<float>(resolution.y), 0.0f, 1.0f);
    std::vector<VkRect2D> scissors;
    scissors.emplace_back(VkRect2D{{0, 0}, {resolution.x, resolution.y}});
    setViewports(std::move(viewports));
    setScissors(std::move(scissors));
}

////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////

uint32_t DisplayMaterialInstance::getSetIndex() const { return 0; }

sol::Texture2D* DisplayMaterialInstance::getCombinedImageSamplerData(size_t binding) const { return texture; }

////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////

void DisplayMaterialInstance::setTexture(sol::Texture2D* tex) { texture = tex; }
