#include "pathtracing/materials/viewer_textures_material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/materials/camera_material.h"

ViewerTexturesMaterial::ViewerTexturesMaterial(sol::VulkanShaderModuleSharedPtr vertexModule,
                                               sol::VulkanShaderModuleSharedPtr fragmentModule) :
    ForwardMaterial(std::move(vertexModule), std::move(fragmentModule))
{
    auto& ub0                 = layout.addUniformBuffer();
    ub0.name                  = "CameraTransform";
    ub0.set                   = 0;
    ub0.binding               = 0;
    ub0.size                  = sizeof(CameraMaterialData);
    ub0.count                 = 1;
    ub0.stages                = VK_SHADER_STAGE_VERTEX_BIT;
    ub0.usage.updateDetection = sol::MaterialLayout::UpdateDetection::Automatic;
    ub0.sharing.method        = sol::ForwardMaterialLayout::SharingMethod::None;
    ub0.sharing.count         = 1;

    auto& pc  = layout.addPushConstant();
    pc.name   = "ModelTransform";
    pc.offset = 0;
    pc.size   = sizeof(float) * 16;
    pc.stages = VK_SHADER_STAGE_VERTEX_BIT;

    auto& activeTexture   = layout.addUniformBuffer();
    activeTexture.name    = "activeTexture";
    activeTexture.set     = 1;
    activeTexture.binding = 0;
    activeTexture.count   = 1;
    activeTexture.size    = sizeof(int32_t);
    activeTexture.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& texDiffuse   = layout.addSampler();
    texDiffuse.name    = "texDiffuse";
    texDiffuse.set     = 1;
    texDiffuse.binding = 1;
    texDiffuse.count   = 1;
    texDiffuse.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& texNormal   = layout.addSampler();
    texNormal.name    = "texNormal";
    texNormal.set     = 1;
    texNormal.binding = 2;
    texNormal.count   = 1;
    texNormal.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& texMetallicRoughness   = layout.addSampler();
    texMetallicRoughness.name    = "texMetallicRoughness";
    texMetallicRoughness.set     = 1;
    texMetallicRoughness.binding = 3;
    texMetallicRoughness.count   = 1;
    texMetallicRoughness.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& texOcclusion   = layout.addSampler();
    texOcclusion.name    = "texOcclusion";
    texOcclusion.set     = 1;
    texOcclusion.binding = 4;
    texOcclusion.count   = 1;
    texOcclusion.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    auto& texEmissive   = layout.addSampler();
    texEmissive.name    = "texEmissive";
    texEmissive.set     = 1;
    texEmissive.binding = 5;
    texEmissive.count   = 1;
    texEmissive.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkPipelineColorBlendAttachmentState colorBlending{};
    colorBlending.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlending.blendEnable = VK_FALSE;
    layout.addColorBlending(colorBlending);

    layout.finalize();
}

ViewerTexturesMaterial::~ViewerTexturesMaterial() noexcept = default;

ViewerTexturesMaterialInstance::ViewerTexturesMaterialInstance(sol::Texture2D& texDiffuse,
                                                               sol::Texture2D& texNormal,
                                                               sol::Texture2D& texMetallicRoughness,
                                                               sol::Texture2D& texOcclusion,
                                                               sol::Texture2D& texEmissive) :
    ForwardMaterialInstance(),
    diffuseTexture(&texDiffuse),
    normalTexture(&texNormal),
    metallicRoughnessTexture(&texMetallicRoughness),
    occlusionTexture(&texOcclusion),
    emissiveTexture(&texEmissive)
{
}

ViewerTexturesMaterialInstance::~ViewerTexturesMaterialInstance() noexcept = default;

////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////

uint32_t ViewerTexturesMaterialInstance::getSetIndex() const { return 1; }

bool ViewerTexturesMaterialInstance::isUniformBufferStale(size_t binding) const { return true; }

const void* ViewerTexturesMaterialInstance::getUniformBufferData(size_t binding) const
{
    if (binding == 0) return &activeTexture;

    return nullptr;
}

sol::Texture2D* ViewerTexturesMaterialInstance::getTextureData(const size_t binding) const
{
    switch (binding)
    {
    case 1: return diffuseTexture;
    case 2: return normalTexture;
    case 3: return metallicRoughnessTexture;
    case 4: return occlusionTexture;
    case 5: return emissiveTexture;
    default: return nullptr;
    }
}

////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////

void ViewerTexturesMaterialInstance::setActiveTexture(const int32_t index) { activeTexture = index; }