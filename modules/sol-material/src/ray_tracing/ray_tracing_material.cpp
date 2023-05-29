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

    RayTracingMaterial::RayTracingMaterial(VulkanShaderModule* raygenModule,
                                           VulkanShaderModule* missModule,
                                           VulkanShaderModule* closestHitModule,
                                           VulkanShaderModule* anyHitModule,
                                           VulkanShaderModule* intersectionModule) :
        raygenShader(raygenModule),
        missShader(missModule),
        closestHitShader(closestHitModule),
        anyHitShader(anyHitModule),
        intersectionShader(intersectionModule),
        layout(raygenModule->getDevice())
    {
    }

    RayTracingMaterial::~RayTracingMaterial() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& RayTracingMaterial::getDevice() noexcept { return raygenShader->getDevice(); }

    const VulkanDevice& RayTracingMaterial::getDevice() const noexcept { return raygenShader->getDevice(); }

    IRayTracingMaterialManager& RayTracingMaterial::getMaterialManager() noexcept { return *materialManager; }

    const IRayTracingMaterialManager& RayTracingMaterial::getMaterialManager() const noexcept
    {
        return *materialManager;
    }

    bool RayTracingMaterial::hasRaygenShader() const noexcept { return raygenShader; }

    VulkanShaderModule& RayTracingMaterial::getRaygenShader() noexcept { return *raygenShader; }

    const VulkanShaderModule& RayTracingMaterial::getRaygenShader() const noexcept { return *raygenShader; }

    bool RayTracingMaterial::hasMissShader() const noexcept { return missShader; }

    VulkanShaderModule& RayTracingMaterial::getMissShader() noexcept { return *missShader; }

    const VulkanShaderModule& RayTracingMaterial::getMissShader() const noexcept { return *missShader; }

    bool RayTracingMaterial::hasClosestHitShader() const noexcept { return closestHitShader; }

    VulkanShaderModule& RayTracingMaterial::getClosestHitShader() noexcept { return *closestHitShader; }

    const VulkanShaderModule& RayTracingMaterial::getClosestHitShader() const noexcept { return *closestHitShader; }

    bool RayTracingMaterial::hasAnyHitShader() const noexcept { return anyHitShader; }

    VulkanShaderModule& RayTracingMaterial::getAnyHitShader() noexcept { return *anyHitShader; }

    const VulkanShaderModule& RayTracingMaterial::getAnyHitShader() const noexcept { return *anyHitShader; }

    bool RayTracingMaterial::hasIntersectionShader() const noexcept { return intersectionShader; }

    VulkanShaderModule& RayTracingMaterial::getIntersectionShader() noexcept { return *intersectionShader; }

    const VulkanShaderModule& RayTracingMaterial::getIntersectionShader() const noexcept { return *intersectionShader; }

    const MaterialLayout& RayTracingMaterial::getLayout() const noexcept { return layout; }

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
