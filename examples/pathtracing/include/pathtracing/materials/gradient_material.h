#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/compute/compute_material.h"
#include "sol-material/compute/compute_material_instance.h"
#include "sol-texture/texture2d.h"

class GradientMaterial final : public sol::ComputeMaterial
{
public:
    explicit GradientMaterial(sol::VulkanShaderModule& m);
};

class GradientMaterialInstance : public sol::ComputeMaterialInstance
{
public:
    explicit GradientMaterialInstance(sol::Texture2D& tex);

    [[nodiscard]] uint32_t getSetIndex() const override;

    [[nodiscard]] sol::Texture2D* getStorageImageData(const size_t binding) const override;


private:
    sol::Texture2D* texture = nullptr;
};