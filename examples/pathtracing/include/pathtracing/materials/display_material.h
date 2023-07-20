#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol-core/fwd.h"
#include "sol-material/graphics/graphics_material.h"
#include "sol-material/graphics/graphics_material_instance.h"
#include "sol-texture/texture2d.h"

class DisplayMaterial : public sol::GraphicsMaterial
{
public:
    DisplayMaterial(sol::MeshLayout&         mLayout,
                    sol::VulkanShaderModule& vertexModule,
                    sol::VulkanShaderModule& fragmentModule);

    DisplayMaterial(const DisplayMaterial&) = delete;

    DisplayMaterial(DisplayMaterial&&) = delete;

    ~DisplayMaterial() noexcept override = default;

    DisplayMaterial& operator=(const DisplayMaterial&) = delete;

    DisplayMaterial& operator=(DisplayMaterial&&) = delete;
};

class DisplayMaterialInstance : public sol::GraphicsMaterialInstance
{
public:
    DisplayMaterialInstance() = default;

    DisplayMaterialInstance(math::uint2 resolution, sol::Texture2D& tex);

    DisplayMaterialInstance(const DisplayMaterialInstance&) = delete;

    DisplayMaterialInstance(DisplayMaterialInstance&&) = delete;

    ~DisplayMaterialInstance() noexcept override = default;

    DisplayMaterialInstance& operator=(const DisplayMaterialInstance&) = delete;

    DisplayMaterialInstance& operator=(DisplayMaterialInstance&&) = delete;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] uint32_t getSetIndex() const override;

    [[nodiscard]] sol::Texture2D* getCombinedImageSamplerData(size_t binding) const override;

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void setTexture(sol::Texture2D* tex);

private:
    sol::Texture2D* texture = nullptr;
};
