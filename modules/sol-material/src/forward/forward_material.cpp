#include "sol-material/forward/forward_material.h"

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

    ForwardMaterial::ForwardMaterial(VulkanShaderModule& vertexModule, VulkanShaderModule& fragmentModule) :
        vertexShader(&vertexModule), fragmentShader(&fragmentModule), layout(vertexShader->getDevice())
    {
        assert(&vertexShader->getDevice() == &fragmentShader->getDevice());
    }

    ForwardMaterial::~ForwardMaterial() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& ForwardMaterial::getDevice() noexcept { return fragmentShader->getDevice(); }

    const VulkanDevice& ForwardMaterial::getDevice() const noexcept { return fragmentShader->getDevice(); }

    IForwardMaterialManager& ForwardMaterial::getMaterialManager() noexcept { return *materialManager; }

    const IForwardMaterialManager& ForwardMaterial::getMaterialManager() const noexcept { return *materialManager; }

    const VulkanShaderModule& ForwardMaterial::getVertexShader() const noexcept { return *vertexShader; }

    const VulkanShaderModule& ForwardMaterial::getFragmentShader() const noexcept { return *fragmentShader; }

    const MaterialLayout& ForwardMaterial::getLayout() const noexcept { return layout; }

    const ForwardMaterialLayout& ForwardMaterial::getForwardLayout() const noexcept { return layout; }

    const MeshLayout* ForwardMaterial::getMeshLayout() const noexcept { return meshLayout; }

    const std::vector<ForwardMaterialInstance*>& ForwardMaterial::getInstances() const noexcept { return instances; }

    int32_t ForwardMaterial::getLayer() const noexcept { return layer; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ForwardMaterial::setMaterialManager(IForwardMaterialManager& manager)
    {
        if (materialManager) throw SolError("Cannot set material manager more than once.");
        materialManager = &manager;
    }

    void ForwardMaterial::setVertexShader(VulkanShaderModule& module) noexcept { vertexShader = &module; }

    void ForwardMaterial::setFragmentShader(VulkanShaderModule& module) noexcept { fragmentShader = &module; }

    void ForwardMaterial::setMeshLayout(MeshLayout& mLayout)
    {
        if (meshLayout) throw SolError("Cannot set mesh layout more than once.");
        meshLayout = &mLayout;
    }

    void ForwardMaterial::addInstance(ForwardMaterialInstance& instance) { instances.emplace_back(&instance); }

    void ForwardMaterial::setLayer(const int32_t l) noexcept { layer = l; }

}  // namespace sol
