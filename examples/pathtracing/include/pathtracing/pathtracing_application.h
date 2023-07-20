#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-material/fwd.h"
#include "sol-mesh/fwd.h"
#include "sol-render/compute/fwd.h"
#include "sol-render/graphics/fwd.h"
#include "sol-render/ray_tracing/fwd.h"
#include "sol-scenegraph/fwd.h"
#include "sol-texture/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/base_application.h"

class PathtracingApplication : public BaseApplication
{
public:
    PathtracingApplication();

    PathtracingApplication(const PathtracingApplication&) = delete;

    PathtracingApplication(PathtracingApplication&&) noexcept = delete;

    ~PathtracingApplication() noexcept override;

    PathtracingApplication& operator=(const PathtracingApplication&) = delete;

    PathtracingApplication& operator=(PathtracingApplication&&) noexcept = delete;

    void initialize() override;

    std::vector<std::string> getExtensions() override;

    void createSupportedFeatures() override;

    void createEnabledFeatures() override;

    std::function<bool(sol::RootVulkanPhysicalDeviceFeatures2&)> getFeatureFilter() override;

private:
    void createRenderingInfos();

    void createCommandBuffers();

    void createSynchronization();

    void createManagers();

    void createShaders();

    void createMeshes();

    void createAccelerationStructures();

    void createTextures();

    void createMaterials();

    void createScenegraphs();

    void createRenderers();

    void createTaskGraph() override;

    sol::ScenegraphPtr scenegraph;

    struct
    {
        std::vector<sol::VulkanCommandBufferPtr> compute;
        std::vector<sol::VulkanCommandBufferPtr> display;
        std::vector<sol::VulkanCommandBufferPtr> rt;
    } commandBuffers;

    struct
    {
        sol::ComputeRendererPtr   renderer;
        sol::ComputeRenderDataPtr renderData;
        sol::ComputeTraverserPtr  traverser;
    } computeRendering;

    struct
    {
        sol::GraphicsRendererPtr                   renderer;
        sol::GraphicsRenderDataPtr                 renderData;
        sol::GraphicsTraverserPtr                  traverser;
        std::vector<sol::GraphicsRenderingInfoPtr> renderingInfo;
    } displayRendering;

    struct
    {
        sol::RayTracingRendererPtr   renderer;
        sol::RayTracingRenderDataPtr renderData;
        sol::RayTracingTraverserPtr  traverser;
    } rtRendering;

    struct
    {
        std::vector<sol::VulkanFencePtr>     inUse;
        std::vector<sol::VulkanSemaphorePtr> finished;
    } computeSync;

    struct
    {
        std::vector<sol::VulkanFencePtr>     inUse;
        std::vector<sol::VulkanSemaphorePtr> finished;
    } rtSync;

    struct
    {
        std::vector<sol::VulkanFencePtr>     inUse;
        std::vector<sol::VulkanSemaphorePtr> finished;
        std::vector<sol::VulkanSemaphorePtr> available;
    } displaySync;

    struct
    {
        sol::VulkanShaderModulePtr gradientCs;
        sol::VulkanShaderModulePtr displayVert;
        sol::VulkanShaderModulePtr displayFrag;
        sol::VulkanShaderModulePtr raygen;
        sol::VulkanShaderModulePtr miss;
        sol::VulkanShaderModulePtr closesthit;
    } shaders;

    struct
    {
        sol::MeshLayout*  displayMeshLayout  = nullptr;
        sol::IMesh*       quadMesh           = nullptr;
        sol::MeshLayout*  triangleMeshLayout = nullptr;
        sol::IndexedMesh* triangleMesh       = nullptr;
    } meshes;

    struct
    {
        sol::VulkanBottomLevelAccelerationStructurePtr bottom;
        sol::VulkanTopLevelAccelerationStructurePtr    top;
    } accelerationStructures;

    sol::GraphicsMaterialManagerPtr   displayMaterialManager;
    sol::MeshManagerPtr               meshManager;
    sol::TextureManagerPtr            textureManager;
    sol::Texture2D*                   texture = nullptr;
    sol::VulkanCommandBufferPtr       computeCommandBuffer;
    sol::ComputeMaterialManagerPtr    computeMaterialManager;
    sol::ComputeMaterial*             computeMaterial         = nullptr;
    sol::ComputeMaterialInstance*     computeMaterialInstance = nullptr;
    sol::GraphicsMaterial*            displayMaterial         = nullptr;
    sol::GraphicsMaterialInstance*    displayMaterialInstance = nullptr;
    sol::RayTracingMaterialManagerPtr rtMaterialManager;
    sol::RayTracingMaterial*          rtMaterial         = nullptr;
    sol::RayTracingMaterialInstance*  rtMaterialInstance = nullptr;
    sol::VulkanShaderBindingTablePtr  sbt;
};
