#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"
#include "sol-mesh/fwd.h"
#include "sol-render/compute/fwd.h"
#include "sol-render/graphics/fwd.h"
#include "sol-scenegraph/fwd.h"
#include "sol-texture/fwd.h"

////////////////////////////////////////////////////////////////
// Example includes.
////////////////////////////////////////////////////////////////

#include "base-application/base_application.h"
#include "base-application/tasks/function_task_resource_list.h"
#include "base-application/tasks/indexed_task_resource.h"
#include "base-application/tasks/pointer_task_resource.h"
#include "base-application/tasks/indexed_task_resource_list.h"
#include "base-application/tasks/non_owning_function_task_resource_list.h"

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

    void createTextures();

    void createMaterials();

    void createScenegraphs();

    void createRenderers();

    void createTaskGraph() override;

    PointerTaskResource<sol::Scenegraph, true> scenegraph;

    struct
    {
        IndexedTaskResource<sol::VulkanCommandBuffer, true> compute;
        IndexedTaskResource<sol::VulkanCommandBuffer, true> display;
    } commandBuffers;

    struct
    {
        PointerTaskResource<sol::ComputeRenderer, true>   renderer;
        PointerTaskResource<sol::ComputeRenderData, true> renderData;
        PointerTaskResource<sol::ComputeTraverser, true>  traverser;
    } computeRendering;

    struct
    {
        PointerTaskResource<sol::GraphicsRenderer, true>      renderer;
        PointerTaskResource<sol::GraphicsRenderData, true>    renderData;
        PointerTaskResource<sol::GraphicsTraverser, true>     traverser;
        IndexedTaskResource<sol::GraphicsRenderingInfo, true> renderingInfo;
    } displayRendering;

    struct
    {
        IndexedTaskResourceList<sol::VulkanFence, true>     inUse;
        IndexedTaskResourceList<sol::VulkanSemaphore, true> finished;
    } computeSync;

    struct
    {
        IndexedTaskResource<sol::VulkanFence, true>             inUse;
        IndexedTaskResourceList<sol::VulkanSemaphore, true>     finished;
        IndexedTaskResource<sol::VulkanSemaphore, true>         available;
        NonOwningFunctionTaskResourceList<sol::VulkanSemaphore> available2;
        FunctionTaskResourceList<VkPipelineStageFlags, false>   flags;
    } displaySync;

    struct
    {
        sol::VulkanShaderModulePtr gradientCs;
        sol::VulkanShaderModulePtr displayVert;
        sol::VulkanShaderModulePtr displayFrag;
    } shaders;

    struct
    {
        sol::MeshLayout*  displayMeshLayout  = nullptr;
        sol::IMesh*       quadMesh           = nullptr;
        sol::MeshLayout*  triangleMeshLayout = nullptr;
        sol::IndexedMesh* triangleMesh       = nullptr;
    } meshes;

    sol::GraphicsMaterialManagerPtr                      displayMaterialManager;
    NonOwningTaskResource<sol::IGraphicsMaterialManager> displayMaterialManager2;
    sol::MeshManagerPtr                                  meshManager;
    sol::TextureManagerPtr                               textureManager;
    sol::Texture2D*                                      texture = nullptr;
    sol::VulkanCommandBufferPtr                          computeCommandBuffer;
    sol::ComputeMaterialManagerPtr                       computeMaterialManager;
    sol::ComputeMaterial*                                computeMaterial         = nullptr;
    sol::ComputeMaterialInstance*                        computeMaterialInstance = nullptr;
    sol::GraphicsMaterial*                               displayMaterial         = nullptr;
    sol::GraphicsMaterialInstance*                       displayMaterialInstance = nullptr;
};
