#include "sol-material/compute/compute_material_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/compute/compute_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialInstance::ComputeMaterialInstance() = default;

    ComputeMaterialInstance::~ComputeMaterialInstance() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IComputeMaterialManager& ComputeMaterialInstance::getMaterialManager() noexcept { return *materialManager; }

    const IComputeMaterialManager& ComputeMaterialInstance::getMaterialManager() const noexcept
    {
        return *materialManager;
    }

    Material& ComputeMaterialInstance::getMaterial() noexcept { return *material; }

    const Material& ComputeMaterialInstance::getMaterial() const noexcept { return *material; }

    ComputeMaterial& ComputeMaterialInstance::getComputeMaterial() noexcept { return *material; }

    const ComputeMaterial& ComputeMaterialInstance::getComputeMaterial() const noexcept { return *material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ComputeMaterialInstance::setMaterialManager(IComputeMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void ComputeMaterialInstance::setMaterial(ComputeMaterial& fwdMaterial)
    {
        if (material) throw SolError("Cannot set material more than once.");
        assert(fwdMaterial.getLayout().isFinalized());
        material = &fwdMaterial;
        material->addInstance(*this);
    }
}  // namespace sol
