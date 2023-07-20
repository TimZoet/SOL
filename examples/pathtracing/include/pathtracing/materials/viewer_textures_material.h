#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/forward/forward_material.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-mesh/fwd.h"
#include "sol-texture/texture2d.h"

class ViewerTexturesMaterial : public sol::ForwardMaterial
{
public:
    ViewerTexturesMaterial() = default;

    ViewerTexturesMaterial(sol::VulkanShaderModuleSharedPtr vertexModule,
                           sol::VulkanShaderModuleSharedPtr fragmentModule);

    ViewerTexturesMaterial(const ViewerTexturesMaterial&) = delete;

    ViewerTexturesMaterial(ViewerTexturesMaterial&&) = delete;

    ~ViewerTexturesMaterial() noexcept override;

    ViewerTexturesMaterial& operator=(const ViewerTexturesMaterial&) = delete;

    ViewerTexturesMaterial& operator=(ViewerTexturesMaterial&&) = delete;
};

class ViewerTexturesMaterialInstance : public sol::ForwardMaterialInstance
{
public:
    ViewerTexturesMaterialInstance() = default;

    ViewerTexturesMaterialInstance(sol::Texture2D& texDiffuse,
                                   sol::Texture2D& texNormal,
                                   sol::Texture2D& texMetallicRoughness,
                                   sol::Texture2D& texOcclusion,
                                   sol::Texture2D& texEmissive);

    ViewerTexturesMaterialInstance(const ViewerTexturesMaterialInstance&) = delete;

    ViewerTexturesMaterialInstance(ViewerTexturesMaterialInstance&&) = delete;

    ~ViewerTexturesMaterialInstance() noexcept override;

    ViewerTexturesMaterialInstance& operator=(const ViewerTexturesMaterialInstance&) = delete;

    ViewerTexturesMaterialInstance& operator=(ViewerTexturesMaterialInstance&&) = delete;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] uint32_t getSetIndex() const override;

    [[nodiscard]] bool isUniformBufferStale(size_t binding) const override;

    [[nodiscard]] const void* getUniformBufferData(size_t binding) const override;

    [[nodiscard]] sol::Texture2D* getTextureData(size_t binding) const override;

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void setActiveTexture(int32_t index);

private:
    int32_t         activeTexture            = 0;
    sol::Texture2D* diffuseTexture           = nullptr;
    sol::Texture2D* normalTexture            = nullptr;
    sol::Texture2D* metallicRoughnessTexture = nullptr;
    sol::Texture2D* occlusionTexture         = nullptr;
    sol::Texture2D* emissiveTexture          = nullptr;
};
