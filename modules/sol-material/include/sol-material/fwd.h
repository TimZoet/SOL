#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class ComputeMaterial;
    class ComputeMaterialInstance;
    class ComputeMaterialLayout;
    class IComputeMaterialManager;
    class ForwardMaterial;
    class ForwardMaterialInstance;
    class ForwardMaterialLayout;
    class IForwardMaterialManager;
    class Material;
    class MaterialInstance;
    class UniformBuffer;
    class UniformBufferManager;

    using ComputeMaterialPtr               = std::unique_ptr<ComputeMaterial>;
    using ComputeMaterialSharedPtr         = std::shared_ptr<ComputeMaterial>;
    using ComputeMaterialInstancePtr       = std::unique_ptr<ComputeMaterialInstance>;
    using ComputeMaterialInstanceSharedPtr = std::shared_ptr<ComputeMaterialInstance>;
    using IComputeMaterialManagerPtr       = std::unique_ptr<IComputeMaterialManager>;
    using IComputeMaterialManagerSharedPtr = std::shared_ptr<IComputeMaterialManager>;
    using ForwardMaterialPtr               = std::unique_ptr<ForwardMaterial>;
    using ForwardMaterialSharedPtr         = std::shared_ptr<ForwardMaterial>;
    using ForwardMaterialInstancePtr       = std::unique_ptr<ForwardMaterialInstance>;
    using ForwardMaterialInstanceSharedPtr = std::shared_ptr<ForwardMaterialInstance>;
    using IForwardMaterialManagerPtr       = std::unique_ptr<IForwardMaterialManager>;
    using IForwardMaterialManagerSharedPtr = std::shared_ptr<IForwardMaterialManager>;
    using MaterialPtr                      = std::unique_ptr<Material>;
    using MaterialSharedPtr                = std::shared_ptr<Material>;
    using MaterialInstancePtr              = std::unique_ptr<MaterialInstance>;
    using MaterialInstanceSharedPtr        = std::shared_ptr<MaterialInstance>;
    using UniformBufferPtr                 = std::unique_ptr<UniformBuffer>;
    using UniformBufferSharedPtr           = std::shared_ptr<UniformBuffer>;
    using UniformBufferManagerPtr          = std::unique_ptr<UniformBufferManager>;
    using UniformBufferManagerSharedPtr    = std::shared_ptr<UniformBufferManager>;
}  // namespace sol
