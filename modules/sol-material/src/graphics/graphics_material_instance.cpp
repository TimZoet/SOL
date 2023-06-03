#include "sol-material/graphics/graphics_material_instance.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsMaterialInstance::GraphicsMaterialInstance() = default;

    GraphicsMaterialInstance::~GraphicsMaterialInstance() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IGraphicsMaterialManager& GraphicsMaterialInstance::getMaterialManager() noexcept { return *materialManager; }

    const IGraphicsMaterialManager& GraphicsMaterialInstance::getMaterialManager() const noexcept
    {
        return *materialManager;
    }

    Material& GraphicsMaterialInstance::getMaterial() noexcept { return *material; }

    const Material& GraphicsMaterialInstance::getMaterial() const noexcept { return *material; }

    GraphicsMaterial& GraphicsMaterialInstance::getGraphicsMaterial() noexcept { return *material; }

    const GraphicsMaterial& GraphicsMaterialInstance::getGraphicsMaterial() const noexcept { return *material; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialInstance::setMaterialManager(IGraphicsMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void GraphicsMaterialInstance::setMaterial(GraphicsMaterial& fwdMaterial)
    {
        if (material) throw SolError("Cannot set material more than once.");
        assert(fwdMaterial.getLayout().isFinalized());
        assert(fwdMaterial.getMeshLayout());
        assert(fwdMaterial.getMeshLayout()->isFinalized());
        material = &fwdMaterial;
        material->addInstance(*this);
    }
}  // namespace sol
