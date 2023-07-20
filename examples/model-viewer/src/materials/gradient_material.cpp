#include "model-viewer/materials/gradient_material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////


GradientMaterial::GradientMaterial(sol::VulkanShaderModule& m) : ComputeMaterial(m)
{
    sol::MaterialLayoutDescription desc;
    auto&                          b = desc.storageImages.emplace_back();
    b.name                           = "texture";
    b.set                            = 0;
    b.binding                        = 0;
    b.stages                         = VK_SHADER_STAGE_COMPUTE_BIT;

    layout.finalize(std::move(desc));
}


GradientMaterialInstance::GradientMaterialInstance(sol::Texture2D& tex) : texture(&tex) {}

uint32_t GradientMaterialInstance::getSetIndex() const { return 0; }

sol::Texture2D* GradientMaterialInstance::getStorageImageData(const size_t binding) const
{
    if (binding == 0) return texture;

    return nullptr;
}
