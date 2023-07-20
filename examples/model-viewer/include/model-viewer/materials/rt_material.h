#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/ray_tracing/ray_tracing_material.h"
#include "sol-material/ray_tracing/ray_tracing_material_instance.h"
#include "sol-texture/texture2d.h"

class RTMaterial final : public sol::RayTracingMaterial
{
public:
    RTMaterial(sol::VulkanShaderModule& raygenModule,
               sol::VulkanShaderModule& missModule,
               sol::VulkanShaderModule& closestHitModule);
};

class RTMaterialInstance final : public sol::RayTracingMaterialInstance
{
public:
    RTMaterialInstance(sol::VulkanTopLevelAccelerationStructure& as, sol::Texture2D& tex);

    [[nodiscard]] uint32_t getSetIndex() const override;

    sol::VulkanTopLevelAccelerationStructure* getAccelerationStructureData(const size_t binding) const override;

    [[nodiscard]] sol::Texture2D* getStorageImageData(const size_t binding) const override;

private:
    sol::VulkanTopLevelAccelerationStructure* accelerationStructure = nullptr;
    sol::Texture2D*                           texture               = nullptr;
};
