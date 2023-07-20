#include "pathtracing/materials/rt_material.h"

#include "sol-core/vulkan_shader_module.h"


RTMaterial::RTMaterial(sol::VulkanShaderModule& raygenModule,
                       sol::VulkanShaderModule& missModule,
                       sol::VulkanShaderModule& closestHitModule) :
    RayTracingMaterial(raygenModule.getDevice())
{
    setRaygenShader(raygenModule);
    addMissShader(missModule);
    addClosestHitShader(closestHitModule);

    sol::MaterialLayoutDescription desc;

    auto& as = desc.accelerationStructures.emplace_back();
    as.set = 0;
    as.binding = 0;
    as.stages = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    auto& b = desc.storageImages.emplace_back();
    b.name = "texture";
    b.set = 0;
    b.binding = 1;
    b.stages = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    layout.finalize(std::move(desc));
}

RTMaterialInstance::RTMaterialInstance(sol::VulkanTopLevelAccelerationStructure& as, sol::Texture2D& tex) :
    accelerationStructure(&as), texture(&tex)
{
}

uint32_t RTMaterialInstance::getSetIndex() const { return 0; }

sol::VulkanTopLevelAccelerationStructure* RTMaterialInstance::getAccelerationStructureData(const size_t binding) const
{
    if (binding == 0) return accelerationStructure;

    return nullptr;
}

sol::Texture2D* RTMaterialInstance::getStorageImageData(const size_t binding) const
{
    if (binding == 1) return texture;

    return nullptr;
}
