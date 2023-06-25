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

    RayTracingMaterial::~RayTracingMaterial() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IRayTracingMaterialManager& RayTracingMaterial::getMaterialManager() noexcept { return *materialManager; }

    const IRayTracingMaterialManager& RayTracingMaterial::getMaterialManager() const noexcept
    {
        return *materialManager;
    }

    VulkanShaderModule* RayTracingMaterial::getRaygenShader() const noexcept { return raygenShader; }

    const std::vector<VulkanShaderModule*>& RayTracingMaterial::getMissShaders() const noexcept { return missShaders; }

    const std::vector<VulkanShaderModule*>& RayTracingMaterial::getClosestHitShaders() const noexcept
    {
        return closestHitShaders;
    }

    const std::vector<VulkanShaderModule*>& RayTracingMaterial::getAnyHitShaders() const noexcept
    {
        return anyHitShaders;
    }

    const std::vector<VulkanShaderModule*>& RayTracingMaterial::getIntersectionShaders() const noexcept
    {
        return intersectionShaders;
    }

    const std::vector<VulkanShaderModule*>& RayTracingMaterial::getCallableShaders() const noexcept
    {
        return callableShaders;
    }

    const std::vector<VulkanRayTracingPipeline::HitShaderGroup>& RayTracingMaterial::getHitGroups() const noexcept
    {
        return hitGroups;
    }

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

    void RayTracingMaterial::setRaygenShader(VulkanShaderModule& shader) noexcept { raygenShader = &shader; }

    void RayTracingMaterial::addMissShader(VulkanShaderModule& shader) noexcept { missShaders.emplace_back(&shader); }

    void RayTracingMaterial::addClosestHitShader(VulkanShaderModule& shader) noexcept
    {
        closestHitShaders.emplace_back(&shader);
    }

    void RayTracingMaterial::addAnyHitShader(VulkanShaderModule& shader) noexcept
    {
        anyHitShaders.emplace_back(&shader);
    }

    void RayTracingMaterial::addIntersectionShader(VulkanShaderModule& shader) noexcept
    {
        intersectionShaders.emplace_back(&shader);
    }

    void RayTracingMaterial::addCallableShader(VulkanShaderModule& shader) noexcept
    {
        callableShaders.emplace_back(&shader);
    }

    void RayTracingMaterial::addHitGroup(std::optional<uint32_t> closest,
                                         std::optional<uint32_t> any,
                                         std::optional<uint32_t> intersection)
    {
        hitGroups.emplace_back(closest, any, intersection);
    }

    void RayTracingMaterial::addInstance(RayTracingMaterialInstance& instance) { instances.emplace_back(&instance); }
}  // namespace sol
