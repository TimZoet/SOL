#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>


////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-command/fwd.h"
#include "sol-core/fwd.h"
#include "sol-material/fwd.h"
#include "sol-memory/fwd.h"
#include "sol-mesh/fwd.h"
#include "sol-render/forward/fwd.h"
#include "sol-scenegraph/fwd.h"
#include "sol-texture/fwd.h"
#include "sol-window/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/materials/camera_material.h"
#include "pathtracing/materials/display_material.h"
#include "pathtracing/materials/gui_transform_node.h"

struct GlobalState
{
    uint32_t                     width  = 0;
    uint32_t                     height = 0;
    sol::WindowPtr               window;
    sol::VulkanInstancePtr       instance;
    sol::VulkanSurfacePtr        surface;
    sol::VulkanPhysicalDevicePtr physicalDevice;
    sol::VulkanDevicePtr         device;
    sol::VulkanSwapchainPtr      swapchain;
    sol::MemoryManagerPtr        memoryManager;

    //
    // Command pool.
    //

    sol::VulkanCommandPoolPtr       commandPool;
    sol::VulkanCommandBufferListPtr guiCommandBuffers;
    sol::VulkanCommandBufferListPtr viewerCommandBuffers;
    sol::VulkanCommandBufferListPtr displayCommandBuffers;

    //
    // Mesh.
    //

    sol::MeshManagerPtr guiMeshManager;
    sol::MeshManagerPtr viewerMeshManager;
    sol::MeshManagerPtr displayMeshManager;
    sol::MeshLayout*    guiMeshLayout     = nullptr;
    sol::MeshLayout*    viewerMeshLayout  = nullptr;
    sol::MeshLayout*    displayMeshLayout = nullptr;
    sol::IndexedMesh*   quadMesh          = nullptr;

    //
    // Texture.
    //

    sol::TextureManagerPtr guiTextureManager;
    sol::TextureManagerPtr viewerTextureManager;
    sol::TextureManagerPtr displayTextureManager;
    sol::Texture2D*        guiFontTexture       = nullptr;
    sol::Texture2D*        viewerMissingTexture = nullptr;

    //
    // Material.
    //

    sol::ForwardMaterialManagerPtr guiMtlManager;
    sol::ForwardMaterialManagerPtr viewerMtlManager;
    sol::ForwardMaterialManagerPtr displayMtlManager;
    sol::ShaderCachePtr            shaderCache;

    //
    // Render.
    //

    sol::ForwardRenderDataPtr guiRenderData;
    sol::ForwardRenderDataPtr viewerRenderData;
    sol::ForwardRenderDataPtr displayRenderData;
    sol::ForwardTraverserPtr  guiTraverser;
    sol::ForwardTraverserPtr  viewerTraverser;
    sol::ForwardTraverserPtr  displayTraverser;
    sol::VulkanRenderPassPtr  guiRenderPass;
    sol::VulkanRenderPassPtr  viewerRenderPass;
    sol::VulkanRenderPassPtr  displayRenderPass;
    sol::ForwardRendererPtr   guiRenderer;
    sol::ForwardRendererPtr   viewerRenderer;
    sol::ForwardRendererPtr   displayRenderer;

    //
    // ...
    //

    sol::ForwardMaterial*         guiMtl                   = nullptr;
    sol::ForwardMaterialInstance* guiMtlInstance           = nullptr;
    sol::ForwardMaterial*         viewerAttributesMtl      = nullptr;
    sol::ForwardMaterial*         viewerLitMtl             = nullptr;
    sol::ForwardMaterial*         viewerTexturesMtl        = nullptr;
    CameraMaterialInstance*       viewerMtlInstance        = nullptr;
    DisplayMaterialInstance*      displayMtlInstanceGui    = nullptr;
    DisplayMaterialInstance*      displayMtlInstanceViewer = nullptr;

    //
    // Framebuffers.
    //

    std::vector<sol::Texture2D*>           guiRenderTextures;
    std::vector<sol::Texture2D*>           viewerRenderTextures;
    std::vector<sol::VulkanFramebufferPtr> guiFramebuffers;
    std::vector<sol::VulkanFramebufferPtr> viewerFramebuffers;
    std::vector<sol::VulkanFramebufferPtr> displayFramebuffers;

    //
    // Scenegraph.
    //

    sol::ScenegraphPtr        guiScenegraph;
    sol::ScenegraphPtr        viewerScenegraph;
    sol::ScenegraphPtr        displayScenegraph;
    sol::ForwardMaterialNode* guiMaterialNode  = nullptr;
    GuiTransformNode*         guiTransformNode = nullptr;
    sol::ForwardMaterialNode* viewerRootNode   = nullptr;

    //
    // Sync.
    //

    std::vector<sol::VulkanFencePtr>     guiCBInUseFences;
    std::vector<sol::VulkanSemaphorePtr> guiRenderFinishedSemaphores;
    std::vector<sol::VulkanFencePtr>     viewerCBInUseFences;
    std::vector<sol::VulkanSemaphorePtr> viewerRenderFinishedSemaphores;
    std::vector<sol::VulkanFencePtr>     displayCBInUseFences;
    std::vector<sol::VulkanSemaphorePtr> displayRenderFinishedSemaphores;
    std::vector<sol::VulkanSemaphorePtr> displayimageAvailableSemaphores;
    std::vector<sol::VulkanFence*>       displayInUseFences;

    //
    // Commands.
    //

    sol::CommandQueuePtr commandQueue;
    sol::ICommand*       pollCmd         = nullptr;
    sol::ICommand*       guiFrameCmd     = nullptr;
    sol::ICommand*       guiSubmitCmd    = nullptr;
    sol::ICommand*       viewerSubmitCmd = nullptr;

    //
    // Frames.
    //

    const uint32_t maxFrames = 2;
    uint32_t       frameIdx  = 0;
    uint32_t       imageIdx  = 0;

    //
    // Scene.
    //

    std::vector<sol::IndexedMesh*>             sceneMeshes;
    std::vector<sol::Image2D*>                 sceneImages;
    std::vector<sol::Texture2D*>               sceneTextures;
    std::vector<sol::ForwardMaterialInstance*> sceneMaterials;

    //
    // ImGui.
    //

    enum ViewMode : int32_t
    {
        Attributes = 1,
        Lit        = 2,
        Textures   = 4
    };

    enum ActiveAttribute : int32_t
    {

    };

    enum ActiveTexture : int32_t
    {

    };
    ImGuiIO*                 imgui = nullptr;
    std::string              importFilename;
    ViewMode                 viewMode            = ViewMode::Textures;
    bool                     updateViewMode      = false;
    int32_t                  activeAttribute     = 0;
    int32_t                  activeTexture       = 0;
    bool                     importButtonPressed = false;
    std::vector<uuids::uuid> oldMeshes;
};