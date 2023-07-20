#include "pathtracing/materials/viewer_attributes_material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/materials/camera_material.h"

ViewerAttributesMaterial::ViewerAttributesMaterial(sol::VulkanShaderModuleSharedPtr vertexModule,
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

    auto& activeAttribute   = layout.addUniformBuffer();
    activeAttribute.name    = "activeAttribute";
    activeAttribute.set     = 1;
    activeAttribute.binding = 0;
    activeAttribute.count   = 1;
    activeAttribute.size    = sizeof(int32_t);
    activeAttribute.stages  = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkPipelineColorBlendAttachmentState colorBlending{};
    colorBlending.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlending.blendEnable = VK_FALSE;
    layout.addColorBlending(colorBlending);

    layout.finalize();
}

ViewerAttributesMaterial::~ViewerAttributesMaterial() noexcept = default;

ViewerAttributesMaterialInstance::~ViewerAttributesMaterialInstance() noexcept = default;

////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////

uint32_t ViewerAttributesMaterialInstance::getSetIndex() const { return 1; }

bool ViewerAttributesMaterialInstance::isUniformBufferStale(size_t binding) const { return true; }

const void* ViewerAttributesMaterialInstance::getUniformBufferData(size_t binding) const
{
    if (binding == 0) return &activeAttribute;

    return nullptr;
}

sol::Texture2D* ViewerAttributesMaterialInstance::getTextureData(const size_t binding) const { return nullptr; }

////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////

void ViewerAttributesMaterialInstance::setActiveAttribute(const int32_t index) { activeAttribute = index; }
