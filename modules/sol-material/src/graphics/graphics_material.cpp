#include "sol-material/graphics/graphics_material.h"

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

    GraphicsMaterial::GraphicsMaterial(VulkanShaderModule& vertexModule, VulkanShaderModule& fragmentModule) :
        vertexShader(&vertexModule), fragmentShader(&fragmentModule), layout(vertexShader->getDevice())
    {
        assert(&vertexShader->getDevice() == &fragmentShader->getDevice());
    }

    GraphicsMaterial::~GraphicsMaterial() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& GraphicsMaterial::getDevice() noexcept { return fragmentShader->getDevice(); }

    const VulkanDevice& GraphicsMaterial::getDevice() const noexcept { return fragmentShader->getDevice(); }

    IGraphicsMaterialManager& GraphicsMaterial::getMaterialManager() noexcept { return *materialManager; }

    const IGraphicsMaterialManager& GraphicsMaterial::getMaterialManager() const noexcept { return *materialManager; }

    VulkanShaderModule& GraphicsMaterial::getVertexShader() const noexcept { return *vertexShader; }

    VulkanShaderModule& GraphicsMaterial::getFragmentShader() const noexcept { return *fragmentShader; }

    const MaterialLayout& GraphicsMaterial::getLayout() const noexcept { return layout; }

    const GraphicsMaterialLayout& GraphicsMaterial::getGraphicsLayout() const noexcept { return layout; }

    const std::vector<GraphicsMaterialInstance*>& GraphicsMaterial::getInstances() const noexcept { return instances; }

    int32_t GraphicsMaterial::getLayer() const noexcept { return layer; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterial::setMaterialManager(IGraphicsMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void GraphicsMaterial::setVertexShader(VulkanShaderModule& module) noexcept { vertexShader = &module; }

    void GraphicsMaterial::setFragmentShader(VulkanShaderModule& module) noexcept { fragmentShader = &module; }

    void GraphicsMaterial::addInstance(GraphicsMaterialInstance& instance) { instances.emplace_back(&instance); }

    void GraphicsMaterial::setLayer(const int32_t l) noexcept { layer = l; }
}  // namespace sol
