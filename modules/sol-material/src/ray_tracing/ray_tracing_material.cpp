#include "sol-material/ray_tracing/ray_tracing_material.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_shader_module.h"
#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterial::RayTracingMaterial(VulkanDevice& device) : Material(device), layout(getDevice()) {}

    RayTracingMaterial::RayTracingMaterial(VulkanShaderModule* raygenModule,
                                           VulkanShaderModule* missModule,
                                           VulkanShaderModule* closestHitModule,
                                           VulkanShaderModule* anyHitModule,
                                           VulkanShaderModule* intersectionModule) :
        Material(raygenModule->getDevice()),
        raygenShader(raygenModule),
        missShader(missModule),
        closestHitShader(closestHitModule),
        anyHitShader(anyHitModule),
        intersectionShader(intersectionModule),
        layout(getDevice())
    {
    }

    RayTracingMaterial::~RayTracingMaterial() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IRayTracingMaterialManager& RayTracingMaterial::getMaterialManager() noexcept { return *materialManager; }

    const IRayTracingMaterialManager& RayTracingMaterial::getMaterialManager() const noexcept
    {
        return *materialManager;
    }

    bool RayTracingMaterial::hasRaygenShader() const noexcept { return raygenShader; }

    VulkanShaderModule& RayTracingMaterial::getRaygenShader() const noexcept { return *raygenShader; }

    bool RayTracingMaterial::hasMissShader() const noexcept { return missShader; }

    VulkanShaderModule& RayTracingMaterial::getMissShader() const noexcept { return *missShader; }

    bool RayTracingMaterial::hasClosestHitShader() const noexcept { return closestHitShader; }

    VulkanShaderModule& RayTracingMaterial::getClosestHitShader() const noexcept { return *closestHitShader; }

    bool RayTracingMaterial::hasAnyHitShader() const noexcept { return anyHitShader; }

    VulkanShaderModule& RayTracingMaterial::getAnyHitShader() const noexcept { return *anyHitShader; }

    bool RayTracingMaterial::hasIntersectionShader() const noexcept { return intersectionShader; }

    VulkanShaderModule& RayTracingMaterial::getIntersectionShader() const noexcept { return *intersectionShader; }

    MaterialLayout& RayTracingMaterial::getLayout() noexcept { return layout; }

    const MaterialLayout& RayTracingMaterial::getLayout() const noexcept { return layout; }

    RayTracingMaterialLayout& RayTracingMaterial::getRayTracingLayout() noexcept { return layout; }

    const RayTracingMaterialLayout& RayTracingMaterial::getRayTracingLayout() const noexcept { return layout; }

    const std::vector<RayTracingMaterialInstance*>& RayTracingMaterial::getInstances() const noexcept
    {
        return instances;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RayTracingMaterial::setMaterialManager(IRayTracingMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void RayTracingMaterial::addInstance(RayTracingMaterialInstance& instance) { instances.emplace_back(&instance); }
}  // namespace sol
