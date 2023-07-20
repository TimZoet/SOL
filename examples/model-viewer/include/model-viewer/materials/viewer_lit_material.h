#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/forward/forward_material.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-mesh/fwd.h"
#include "sol-texture/texture2d.h"

class ViewerLitMaterial : public sol::ForwardMaterial
{
public:
    ViewerLitMaterial() = default;

    ViewerLitMaterial(sol::VulkanShaderModuleSharedPtr vertexModule, sol::VulkanShaderModuleSharedPtr fragmentModule);

    ViewerLitMaterial(const ViewerLitMaterial&) = delete;

    ViewerLitMaterial(ViewerLitMaterial&&) = delete;

    ~ViewerLitMaterial() noexcept override;

    ViewerLitMaterial& operator=(const ViewerLitMaterial&) = delete;

    ViewerLitMaterial& operator=(ViewerLitMaterial&&) = delete;
};

class ViewerLitMaterialInstance : public sol::ForwardMaterialInstance
{
public:
    ViewerLitMaterialInstance() = default;

    ViewerLitMaterialInstance(sol::Texture2D& texDiffuse,
                              sol::Texture2D& texNormal,
                              sol::Texture2D& texMetallicRoughness,
                              sol::Texture2D& texOcclusion,
                              sol::Texture2D& texEmissive);

    ViewerLitMaterialInstance(const ViewerLitMaterialInstance&) = delete;

    ViewerLitMaterialInstance(ViewerLitMaterialInstance&&) = delete;

    ~ViewerLitMaterialInstance() noexcept override;

    ViewerLitMaterialInstance& operator=(const ViewerLitMaterialInstance&) = delete;

    ViewerLitMaterialInstance& operator=(ViewerLitMaterialInstance&&) = delete;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] uint32_t getSetIndex() const override;

    [[nodiscard]] bool isUniformBufferStale(size_t binding) const override;

    [[nodiscard]] const void* getUniformBufferData(size_t binding) const override;

    [[nodiscard]] sol::Texture2D* getTextureData(size_t binding) const override;

private:
    sol::Texture2D* diffuseTexture           = nullptr;
    sol::Texture2D* normalTexture            = nullptr;
    sol::Texture2D* metallicRoughnessTexture = nullptr;
    sol::Texture2D* occlusionTexture         = nullptr;
    sol::Texture2D* emissiveTexture          = nullptr;
};
