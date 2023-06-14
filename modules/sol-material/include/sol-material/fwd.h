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
    class GraphicsMaterial;
    class GraphicsMaterialInstance;
    class GraphicsMaterialLayout;
    class IGraphicsMaterialManager;
    class Material;
    class MaterialInstance;
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
    using GraphicsMaterialInstancePtr         = std::unique_ptr<GraphicsMaterialInstance>;
    using GraphicsMaterialInstanceSharedPtr   = std::shared_ptr<GraphicsMaterialInstance>;
    using IGraphicsMaterialManagerPtr         = std::unique_ptr<IGraphicsMaterialManager>;
    using IGraphicsMaterialManagerSharedPtr   = std::shared_ptr<IGraphicsMaterialManager>;
    using MaterialPtr                         = std::unique_ptr<Material>;
    using MaterialSharedPtr                   = std::shared_ptr<Material>;
    using MaterialInstancePtr                 = std::unique_ptr<MaterialInstance>;
    using MaterialInstanceSharedPtr           = std::shared_ptr<MaterialInstance>;
    using RayTracingMaterialPtr               = std::unique_ptr<RayTracingMaterial>;
    using RayTracingMaterialSharedPtr         = std::shared_ptr<RayTracingMaterial>;
    using RayTracingMaterialInstancePtr       = std::unique_ptr<RayTracingMaterialInstance>;
    using RayTracingMaterialInstanceSharedPtr = std::shared_ptr<RayTracingMaterialInstance>;
    using IRayTracingMaterialManagerPtr       = std::unique_ptr<IRayTracingMaterialManager>;
    using IRayTracingMaterialManagerSharedPtr = std::shared_ptr<IRayTracingMaterialManager>;
}  // namespace sol
