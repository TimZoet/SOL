#include "sol-material/forward/forward_material_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/forward/forward_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardMaterialInstance::ForwardMaterialInstance() = default;

    ForwardMaterialInstance::~ForwardMaterialInstance() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IForwardMaterialManager& ForwardMaterialInstance::getMaterialManager() noexcept { return *materialManager; }

    const IForwardMaterialManager& ForwardMaterialInstance::getMaterialManager() const noexcept
    {
        return *materialManager;
    }

    Material& ForwardMaterialInstance::getMaterial() noexcept { return *material; }

    const Material& ForwardMaterialInstance::getMaterial() const noexcept { return *material; }

    ForwardMaterial& ForwardMaterialInstance::getForwardMaterial() noexcept { return *material; }

    const ForwardMaterial& ForwardMaterialInstance::getForwardMaterial() const noexcept { return *material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ForwardMaterialInstance::setMaterialManager(IForwardMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void ForwardMaterialInstance::setMaterial(ForwardMaterial& fwdMaterial)
    {
        if (material) throw SolError("Cannot set material more than once.");
        assert(fwdMaterial.getLayout().isFinalized());
        assert(fwdMaterial.getMeshLayout());
        assert(fwdMaterial.getMeshLayout()->isFinalized());
        material = &fwdMaterial;
        material->addInstance(*this);
    }
}  // namespace sol
