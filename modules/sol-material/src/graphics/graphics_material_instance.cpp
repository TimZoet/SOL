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

    std::optional<CullMode> GraphicsMaterialInstance::getCullMode() const noexcept { return cullMode; }

    std::optional<FrontFace> GraphicsMaterialInstance::getFrontFace() const noexcept { return frontFace; }

    const std::optional<std::vector<VkViewport>>& GraphicsMaterialInstance::getViewports() const noexcept
    {
        return viewports;
    }

    const std::optional<std::vector<VkRect2D>>& GraphicsMaterialInstance::getScissors() const noexcept
    {
        return scissors;
    }

    std::optional<PolygonMode> GraphicsMaterialInstance::getPolygonMode() const noexcept { return polygonMode; }

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
        material = &fwdMaterial;
        material->addInstance(*this);
    }

    void GraphicsMaterialInstance::setCullMode(const std::optional<CullMode> value) noexcept { cullMode = value; }

    void GraphicsMaterialInstance::setFrontFace(const std::optional<FrontFace> value) noexcept { frontFace = value; }

    void GraphicsMaterialInstance::setViewports(std::optional<std::vector<VkViewport>> value)
    {
        viewports = std::move(value);
    }

    void GraphicsMaterialInstance::setScissors(std::optional<std::vector<VkRect2D>> value)
    {
        scissors = std::move(value);
    }

    void GraphicsMaterialInstance::setPolygonMode(const std::optional<PolygonMode> value) noexcept
    {
        polygonMode = value;
    }
}  // namespace sol
