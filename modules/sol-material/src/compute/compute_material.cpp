#include "sol-material/compute/compute_material.h"

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

    ComputeMaterial::ComputeMaterial(VulkanShaderModule& computeModule) :

        computeShader(&computeModule),

        layout(computeShader->getDevice())
    {
    }

    ComputeMaterial::~ComputeMaterial() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IComputeMaterialManager& ComputeMaterial::getMaterialManager() noexcept { return *materialManager; }

    const IComputeMaterialManager& ComputeMaterial::getMaterialManager() const noexcept { return *materialManager; }

    VulkanShaderModule& ComputeMaterial::getComputeShader() noexcept { return *computeShader; }

    const VulkanShaderModule& ComputeMaterial::getComputeShader() const noexcept { return *computeShader; }

    const ComputeMaterialLayout& ComputeMaterial::getLayout() const noexcept { return layout; }

    const std::vector<ComputeMaterialInstance*>& ComputeMaterial::getInstances() const noexcept { return instances; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ComputeMaterial::setMaterialManager(IComputeMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void ComputeMaterial::setComputeShader(VulkanShaderModule& module) noexcept { computeShader = &module; }

    void ComputeMaterial::addInstance(ComputeMaterialInstance& instance) { instances.emplace_back(&instance); }
}  // namespace sol