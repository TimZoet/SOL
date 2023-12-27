#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    struct GraphicsDynamicState;

    class ComputeMaterial;
    class ComputeMaterialInstance;
    class ComputeMaterialLayout;
    class IComputeMaterialManager;
    class GraphicsMaterial;
    class GraphicsMaterial2;
    class GraphicsMaterialInstance;
    class GraphicsMaterialInstance2;
    class GraphicsMaterialLayout;
    class IGraphicsMaterialManager;
    class Material;
    class Material2;
    class MaterialInstance;
    class MaterialInstance2;
    class MaterialLayout;
    class MaterialLayoutDescription;
    class RayTracingMaterial;
    class RayTracingMaterialInstance;
    class RayTracingMaterialLayout;
    class IRayTracingMaterialManager;

    using ComputeMaterialPtr                  = std::unique_ptr<ComputeMaterial>;
    using ComputeMaterialSharedPtr            = std::shared_ptr<ComputeMaterial>;
    using ComputeMaterialInstancePtr          = std::unique_ptr<ComputeMaterialInstance>;
    using ComputeMaterialInstanceSharedPtr    = std::shared_ptr<ComputeMaterialInstance>;
    using IComputeMaterialManagerPtr          = std::unique_ptr<IComputeMaterialManager>;
    using IComputeMaterialManagerSharedPtr    = std::shared_ptr<IComputeMaterialManager>;
    using GraphicsMaterialPtr                 = std::unique_ptr<GraphicsMaterial>;
    using GraphicsMaterialSharedPtr           = std::shared_ptr<GraphicsMaterial>;
    using GraphicsMaterial2Ptr                = std::unique_ptr<GraphicsMaterial2>;
    using GraphicsMaterial2SharedPtr          = std::shared_ptr<GraphicsMaterial2>;
    using GraphicsMaterialInstancePtr         = std::unique_ptr<GraphicsMaterialInstance>;
    using GraphicsMaterialInstanceSharedPtr   = std::shared_ptr<GraphicsMaterialInstance>;
    using GraphicsMaterialInstance2Ptr        = std::unique_ptr<GraphicsMaterialInstance2>;
    using GraphicsMaterialInstance2SharedPtr  = std::shared_ptr<GraphicsMaterialInstance2>;
    using IGraphicsMaterialManagerPtr         = std::unique_ptr<IGraphicsMaterialManager>;
    using IGraphicsMaterialManagerSharedPtr   = std::shared_ptr<IGraphicsMaterialManager>;
    using MaterialPtr                         = std::unique_ptr<Material>;
    using MaterialSharedPtr                   = std::shared_ptr<Material>;
    using Material2Ptr                        = std::unique_ptr<Material2>;
    using Material2SharedPtr                  = std::shared_ptr<Material2>;
    using MaterialInstancePtr                 = std::unique_ptr<MaterialInstance>;
    using MaterialInstanceSharedPtr           = std::shared_ptr<MaterialInstance>;
    using MaterialInstance2Ptr                = std::unique_ptr<MaterialInstance2>;
    using MaterialInstance2SharedPtr          = std::shared_ptr<MaterialInstance2>;
    using RayTracingMaterialPtr               = std::unique_ptr<RayTracingMaterial>;
    using RayTracingMaterialSharedPtr         = std::shared_ptr<RayTracingMaterial>;
    using RayTracingMaterialInstancePtr       = std::unique_ptr<RayTracingMaterialInstance>;
    using RayTracingMaterialInstanceSharedPtr = std::shared_ptr<RayTracingMaterialInstance>;
    using IRayTracingMaterialManagerPtr       = std::unique_ptr<IRayTracingMaterialManager>;
    using IRayTracingMaterialManagerSharedPtr = std::shared_ptr<IRayTracingMaterialManager>;
}  // namespace sol
