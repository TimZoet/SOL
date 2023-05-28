#include "sol-material/ray_tracing/ray_tracing_material_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/ray_tracing/ray_tracing_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterialInstance::RayTracingMaterialInstance() = default;

    RayTracingMaterialInstance::~RayTracingMaterialInstance() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IRayTracingMaterialManager& RayTracingMaterialInstance::getMaterialManager() noexcept { return *materialManager; }

    const IRayTracingMaterialManager& RayTracingMaterialInstance::getMaterialManager() const noexcept
    {
        return *materialManager;
    }

    Material& RayTracingMaterialInstance::getMaterial() noexcept { return *material; }

    const Material& RayTracingMaterialInstance::getMaterial() const noexcept { return *material; }

    RayTracingMaterial& RayTracingMaterialInstance::getRayTracingMaterial() noexcept { return *material; }

    const RayTracingMaterial& RayTracingMaterialInstance::getRayTracingMaterial() const noexcept { return *material; }

    Texture2D* RayTracingMaterialInstance::getTextureData(size_t) const { return nullptr; }

    VulkanBuffer* RayTracingMaterialInstance::getStorageBufferData(size_t) const { return nullptr; }

    VulkanTopLevelAccelerationStructure* RayTracingMaterialInstance::getAccelerationStructureData(size_t) const
    {
        return nullptr;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RayTracingMaterialInstance::setMaterialManager(IRayTracingMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void RayTracingMaterialInstance::setMaterial(RayTracingMaterial& rtMaterial)
    {
        if (material) throw SolError("Cannot set material more than once.");
        assert(rtMaterial.getLayout().isFinalized());
        material = &rtMaterial;
        material->addInstance(*this);
    }
}  // namespace sol
