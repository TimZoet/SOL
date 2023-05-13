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

    ForwardMaterial::ForwardMaterial(VulkanShaderModuleSharedPtr vertexModule,
                                     VulkanShaderModuleSharedPtr fragmentModule) :
        vertexShader(std::move(vertexModule)),
        fragmentShader(std::move(fragmentModule)),
        layout(vertexShader->getDevice())
    {
        assert(&vertexShader->getDevice() == &fragmentShader->getDevice());
    }

    ForwardMaterial::~ForwardMaterial() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IForwardMaterialManager& ForwardMaterial::getMaterialManager() noexcept { return *materialManager; }

    const IForwardMaterialManager& ForwardMaterial::getMaterialManager() const noexcept { return *materialManager; }

    const VulkanShaderModuleSharedPtr& ForwardMaterial::getVertexShader() const noexcept { return vertexShader; }

    const VulkanShaderModuleSharedPtr& ForwardMaterial::getFragmentShader() const noexcept { return fragmentShader; }

    const ForwardMaterialLayout& ForwardMaterial::getLayout() const noexcept { return layout; }

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

    void ForwardMaterial::setVertexShader(VulkanShaderModuleSharedPtr module) noexcept
    {
        vertexShader = std::move(module);
    }

    void ForwardMaterial::setFragmentShader(VulkanShaderModuleSharedPtr module) noexcept
    {
        fragmentShader = std::move(module);
    }

    void ForwardMaterial::setMeshLayout(MeshLayout& mLayout)
    {
        if (meshLayout) throw SolError("Cannot set mesh layout more than once.");
        meshLayout = &mLayout;
    }

    void ForwardMaterial::addInstance(ForwardMaterialInstance& instance) { instances.emplace_back(&instance); }

    void ForwardMaterial::setLayer(const int32_t l) noexcept { layer = l; }

}  // namespace sol
