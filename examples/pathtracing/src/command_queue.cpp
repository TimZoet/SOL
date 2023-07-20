#include "pathtracing/command_queue.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <fstream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-command/material/forward/update_forward_material_manager_command.h"
#include "sol-command/mesh/update_mesh_manager_command.h"
#include "sol-command/other/custom_command.h"
#include "sol-command/other/fence_command.h"
#include "sol-command/other/submit_command.h"
#include "sol-command/present/acquire_command.h"
#include "sol-command/present/present_command.h"
#include "sol-command/render/forward/forward_render_command.h"
#include "sol-command/render/forward/forward_traverse_command.h"
#include "sol-command/texture/update_texture_manager_command.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-material/forward/forward_material_manager.h"
#include "sol-memory/memory_manager.h"
#include "sol-mesh/indexed_mesh.h"
#include "sol-mesh/mesh_description.h"
#include "sol-mesh/mesh_manager.h"
#include "sol-mesh/shared_mesh.h"
#include "sol-render/forward/forward_render_data.h"
#include "sol-render/forward/forward_traverser.h"
#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/forward/forward_material_node.h"
#include "sol-texture/texture_manager.h"
#include "sol-window/window.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/imgui_context.h"
#include "pathtracing/imgui_impl_glfw.h"
#include "pathtracing/importer.h"
#include "pathtracing/materials/gui_material.h"
#include "pathtracing/materials/viewer_attributes_material.h"
#include "pathtracing/materials/viewer_lit_material.h"
#include "pathtracing/materials/viewer_textures_material.h"

void createCommandQueue(GlobalState& state)
{
    state.commandQueue = std::make_unique<sol::CommandQueue>();

    auto& pollCommand = state.commandQueue->createCommand<sol::CustomCommand>();
    pollCommand.setName("pollCommand");
    pollCommand.setFunction([] { glfwPollEvents(); });
    state.pollCmd = &pollCommand;

    createGuiCommands(state);
    createViewerCommands(state);
    createDisplayCommands(state);

    state.commandQueue->finalize();
    /*dot::Graph graph;
    state.commandQueue->visualize(graph);
    std::ofstream file("commandqueue.dot");
    graph.write(file);*/
}

void createGuiCommands(GlobalState& state)
{
    auto& imguiFrameCommand = state.commandQueue->createCommand<sol::CustomCommand>();
    imguiFrameCommand.setName("(GUI) ImGui Frame");
    imguiFrameCommand.addDependency(*state.pollCmd);
    imguiFrameCommand.setFunction([&] { imguiFrame(state); });
    state.guiFrameCmd = &imguiFrameCommand;

    auto& imguiUpdateCommand = state.commandQueue->createCommand<sol::CustomCommand>();
    imguiUpdateCommand.setName("(GUI) ImGui Update");
    imguiUpdateCommand.addDependency(imguiFrameCommand);
    imguiUpdateCommand.setFunction([&] { imguiUpdate(state); });

    auto& guiTraverseCommand = state.commandQueue->createCommand<sol::ForwardTraverseCommand>();
    guiTraverseCommand.setName("(GUI)");
    guiTraverseCommand.setRenderData(*state.guiRenderData);
    guiTraverseCommand.setTraverser(*state.guiTraverser);
    guiTraverseCommand.setScenegraph(*state.guiScenegraph);
    guiTraverseCommand.addDependency(imguiUpdateCommand);

    auto& guiAwaitFenceCommand = state.commandQueue->createCommand<sol::FenceCommand>();
    guiAwaitFenceCommand.setName("(GUI) Await Fence");
    guiAwaitFenceCommand.setAction(sol::FenceCommand::Action::Wait | sol::FenceCommand::Action::Reset);
    for (auto& fence : state.guiCBInUseFences) guiAwaitFenceCommand.addFence(*fence);
    guiAwaitFenceCommand.setFenceIndexPtr(&state.frameIdx);
    guiAwaitFenceCommand.addDependency(guiTraverseCommand);

    auto& updateMaterialCommand = state.commandQueue->createCommand<sol::UpdateForwardMaterialManagerCommand>();
    updateMaterialCommand.setName("(GUI)");
    updateMaterialCommand.setMaterialManager(*state.guiMtlManager);
    updateMaterialCommand.setImageIndexPtr(&state.frameIdx);
    updateMaterialCommand.addDependency(guiAwaitFenceCommand);

    auto& updateMeshCommand = state.commandQueue->createCommand<sol::UpdateMeshManagerCommand>();
    updateMeshCommand.setName("(GUI)");
    updateMeshCommand.setMeshManager(*state.guiMeshManager);
    updateMeshCommand.addDependency(updateMaterialCommand);

    auto& updateTextureCommand = state.commandQueue->createCommand<sol::UpdateTextureManagerCommand>();
    updateTextureCommand.setName("(GUI)");
    updateTextureCommand.setTextureManager(*state.guiTextureManager);
    updateTextureCommand.addDependency(updateMeshCommand);

    auto& guiRenderCommand = state.commandQueue->createCommand<sol::ForwardRenderCommand>();
    guiRenderCommand.setName("(GUI)");
    guiRenderCommand.setRenderer(*state.guiRenderer);
    guiRenderCommand.setRenderData(*state.guiRenderData);
    guiRenderCommand.setRenderPass(*state.guiRenderPass);
    guiRenderCommand.setCommandBufferList(*state.guiCommandBuffers);
    // TODO: All this adding by iterating is ugly. It should be possible to do a set/add with a vector.
    for (const auto& fb : state.guiFramebuffers) guiRenderCommand.addFramebuffer(*fb);
    guiRenderCommand.setImageIndexPtr(&state.frameIdx);
    guiRenderCommand.addDependency(guiAwaitFenceCommand);

    auto& guiSubmitCommand = state.commandQueue->createCommand<sol::SubmitCommand>();
    guiSubmitCommand.setName("(GUI)");
    guiSubmitCommand.setQueue(state.memoryManager->getGraphicsQueue());
    guiSubmitCommand.setCommandBufferList(*state.guiCommandBuffers);
    guiSubmitCommand.setCommandBufferIndexPtr(&state.frameIdx);
    for (auto& sem : state.guiRenderFinishedSemaphores) guiSubmitCommand.addSignalSemaphore(*sem);
    guiSubmitCommand.setSignalSemaphoreIndexPtr(&state.frameIdx);
    for (auto& fence : state.guiCBInUseFences) guiSubmitCommand.addSignalFence(*fence);
    guiSubmitCommand.setSignalFenceIndexPtr(&state.frameIdx);
    guiSubmitCommand.addDependency(guiRenderCommand);
    guiSubmitCommand.addDependency(updateTextureCommand);
    state.guiSubmitCmd = &guiSubmitCommand;
}

void createViewerCommands(GlobalState& state)
{
    auto& viewerTraverseCommand = state.commandQueue->createCommand<sol::ForwardTraverseCommand>();
    viewerTraverseCommand.setName("(Viewer)");
    viewerTraverseCommand.setRenderData(*state.viewerRenderData);
    viewerTraverseCommand.setTraverser(*state.viewerTraverser);
    viewerTraverseCommand.setScenegraph(*state.viewerScenegraph);
    viewerTraverseCommand.addDependency(*state.guiFrameCmd);

    auto& viewerAwaitFenceCommand = state.commandQueue->createCommand<sol::FenceCommand>();
    viewerAwaitFenceCommand.setName("(Viewer) Await Fence");
    viewerAwaitFenceCommand.setAction(sol::FenceCommand::Action::Wait | sol::FenceCommand::Action::Reset);
    for (auto& fence : state.viewerCBInUseFences) viewerAwaitFenceCommand.addFence(*fence);
    viewerAwaitFenceCommand.setFenceIndexPtr(&state.frameIdx);
    viewerAwaitFenceCommand.addDependency(viewerTraverseCommand);

    auto& viewerLoadNewModelCommand = state.commandQueue->createCommand<sol::CustomCommand>();
    viewerLoadNewModelCommand.setName("(Viewer) Load Model");
    viewerLoadNewModelCommand.setFunction([&] {
        for (size_t i = 0; i < state.sceneMaterials.size(); i++)
        {
            auto* mtl = state.sceneMaterials[i];
            if (i % 3 == 0)
                static_cast<ViewerAttributesMaterialInstance&>(*mtl).setActiveAttribute(state.activeAttribute);
            else if (i % 3 == 2)
                static_cast<ViewerTexturesMaterialInstance&>(*mtl).setActiveTexture(state.activeTexture);
        }

        if (state.importButtonPressed)
        {
            // Destroy old resources.
            for (const auto* mesh : state.sceneMeshes) state.viewerMeshManager->destroyMesh(mesh->getUuid());
            for (auto* mtl : state.sceneMaterials) state.viewerMtlManager->destroyMaterialInstance(*mtl);
            for (auto* tex : state.sceneTextures) state.viewerTextureManager->destroyTexture2D(*tex);
            for (auto* image : state.sceneImages) state.viewerTextureManager->destroyImage2D(*image);

            // Clear scenegraph.
            state.viewerRootNode->clearChildren();
            state.viewerRenderData->clear();

            importScene(state);
        }
    });
    viewerLoadNewModelCommand.addDependency(viewerAwaitFenceCommand);


    auto& updateMaterialCommand = state.commandQueue->createCommand<
      sol::UpdateForwardMaterialManagerCommand>();  // TODO: Can these 3 manager commands be done in parallel?
    updateMaterialCommand.setName("(Viewer)");
    updateMaterialCommand.setMaterialManager(*state.viewerMtlManager);
    updateMaterialCommand.setImageIndexPtr(&state.frameIdx);
    updateMaterialCommand.addDependency(viewerLoadNewModelCommand);

    auto& updateMeshCommand = state.commandQueue->createCommand<sol::UpdateMeshManagerCommand>();
    updateMeshCommand.setName("(Viewer)");
    updateMeshCommand.setMeshManager(*state.viewerMeshManager);
    updateMeshCommand.addDependency(updateMaterialCommand);

    auto& updateTextureCommand = state.commandQueue->createCommand<sol::UpdateTextureManagerCommand>();
    updateTextureCommand.setName("(Viewer)");
    updateTextureCommand.setTextureManager(*state.viewerTextureManager);
    updateTextureCommand.addDependency(updateMeshCommand);

    auto& viewerRenderCommand = state.commandQueue->createCommand<sol::ForwardRenderCommand>();
    viewerRenderCommand.setName("(Viewer)");
    viewerRenderCommand.setRenderer(*state.viewerRenderer);
    viewerRenderCommand.setRenderData(*state.viewerRenderData);
    viewerRenderCommand.setRenderPass(*state.viewerRenderPass);
    viewerRenderCommand.setCommandBufferList(*state.viewerCommandBuffers);
    for (const auto& fb : state.viewerFramebuffers) viewerRenderCommand.addFramebuffer(*fb);
    viewerRenderCommand.setImageIndexPtr(&state.frameIdx);
    viewerRenderCommand.addDependency(viewerLoadNewModelCommand);

    auto& viewerSubmitCommand = state.commandQueue->createCommand<sol::SubmitCommand>();
    viewerSubmitCommand.setName("(Viewer)");
    viewerSubmitCommand.setQueue(state.memoryManager->getGraphicsQueue());
    viewerSubmitCommand.setCommandBufferList(*state.viewerCommandBuffers);
    viewerSubmitCommand.setCommandBufferIndexPtr(&state.frameIdx);
    for (auto& sem : state.viewerRenderFinishedSemaphores) viewerSubmitCommand.addSignalSemaphore(*sem);
    viewerSubmitCommand.setSignalSemaphoreIndexPtr(&state.frameIdx);
    for (auto& fence : state.viewerCBInUseFences) viewerSubmitCommand.addSignalFence(*fence);
    viewerSubmitCommand.setSignalFenceIndexPtr(&state.frameIdx);
    viewerSubmitCommand.addDependency(viewerRenderCommand);
    viewerSubmitCommand.addDependency(updateTextureCommand);
    state.viewerSubmitCmd = &viewerSubmitCommand;
}

void createDisplayCommands(GlobalState& state)
{
    auto& displayTraverseCommand = state.commandQueue->createCommand<sol::ForwardTraverseCommand>();
    displayTraverseCommand.setName("(Display)");
    displayTraverseCommand.setRenderData(*state.displayRenderData);
    displayTraverseCommand.setTraverser(*state.displayTraverser);
    displayTraverseCommand.setScenegraph(*state.displayScenegraph);
    displayTraverseCommand.addDependency(*state.pollCmd);

    auto& displayAwaitFenceCommand = state.commandQueue->createCommand<sol::FenceCommand>();
    displayAwaitFenceCommand.setName("(Display) Await Fence");
    displayAwaitFenceCommand.setAction(sol::FenceCommand::Action::Wait | sol::FenceCommand::Action::Reset);
    for (auto& fence : state.displayCBInUseFences) displayAwaitFenceCommand.addFence(*fence);
    displayAwaitFenceCommand.setFenceIndexPtr(&state.frameIdx);
    displayAwaitFenceCommand.addDependency(displayTraverseCommand);

    auto& acquireCommand = state.commandQueue->createCommand<sol::AcquireCommand>();
    acquireCommand.setName("(Display)");
    acquireCommand.setSwapchain(*state.swapchain);
    for (auto& sem : state.displayimageAvailableSemaphores) acquireCommand.addSignalSemaphore(*sem);
    acquireCommand.setSignalSemaphoreIndexPtr(&state.frameIdx);
    acquireCommand.setImageIndexPtr(&state.imageIdx);
    acquireCommand.addDependency(displayAwaitFenceCommand);

    auto& assignTexturesCommand = state.commandQueue->createCommand<sol::CustomCommand>();
    assignTexturesCommand.setName("(Display) Assign Textures");
    assignTexturesCommand.addDependency(acquireCommand);
    assignTexturesCommand.setFunction([&] {
        state.displayMtlInstanceGui->setTexture(state.guiRenderTextures[state.frameIdx]);
        state.displayMtlInstanceViewer->setTexture(state.viewerRenderTextures[state.frameIdx]);
    });

    auto& displayRenderCommand = state.commandQueue->createCommand<sol::ForwardRenderCommand>();
    displayRenderCommand.setName("(Display)");
    displayRenderCommand.setRenderer(*state.displayRenderer);
    displayRenderCommand.setRenderData(*state.displayRenderData);
    displayRenderCommand.setRenderPass(*state.displayRenderPass);
    displayRenderCommand.setCommandBufferList(*state.displayCommandBuffers);
    for (const auto& fb : state.displayFramebuffers) displayRenderCommand.addFramebuffer(*fb);
    displayRenderCommand.setImageIndexPtr(&state.imageIdx);
    displayRenderCommand.addDependency(assignTexturesCommand);

    auto& displayUpdateDataCommand = state.commandQueue->createCommand<sol::UpdateForwardMaterialManagerCommand>();
    displayUpdateDataCommand.setName("(Display)");
    displayUpdateDataCommand.setMaterialManager(*state.displayMtlManager);
    displayUpdateDataCommand.setImageIndexPtr(&state.imageIdx);
    displayUpdateDataCommand.addDependency(displayAwaitFenceCommand);

    auto& displayUpdateInUseCommand = state.commandQueue->createCommand<sol::CustomCommand>();
    displayUpdateInUseCommand.setName("(Display) Update Fence");
    displayUpdateInUseCommand.setFunction([&] {
        if (state.displayInUseFences[state.imageIdx] != nullptr)
            vkWaitForFences(state.displayInUseFences[state.imageIdx]->getSettings().device,
                            1,
                            &state.displayInUseFences[state.imageIdx]->get(),
                            VK_TRUE,
                            UINT64_MAX);
        state.displayInUseFences[state.imageIdx] = state.displayCBInUseFences[state.frameIdx].get();
    });
    displayUpdateInUseCommand.addDependency(displayUpdateDataCommand);

    auto& displaySubmitCommand = state.commandQueue->createCommand<sol::SubmitCommand>();
    displaySubmitCommand.setName("(Display)");
    displaySubmitCommand.setQueue(state.memoryManager->getGraphicsQueue());
    displaySubmitCommand.setCommandBufferList(*state.displayCommandBuffers);
    displaySubmitCommand.setCommandBufferIndexPtr(&state.imageIdx);
    for (size_t i = 0; i < state.maxFrames; i++)
    {
        displaySubmitCommand.addWaitSemaphore(*state.guiRenderFinishedSemaphores[i],
                                              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        displaySubmitCommand.addWaitSemaphore(*state.viewerRenderFinishedSemaphores[i],
                                              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        displaySubmitCommand.addWaitSemaphore(*state.displayimageAvailableSemaphores[i],
                                              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }
    displaySubmitCommand.setWaitSemaphoreFunction([&state](const uint32_t i) { return i / 3 == state.frameIdx; });
    for (auto& sem : state.displayRenderFinishedSemaphores) displaySubmitCommand.addSignalSemaphore(*sem);
    displaySubmitCommand.setSignalSemaphoreIndexPtr(&state.frameIdx);
    for (auto& fence : state.displayCBInUseFences) displaySubmitCommand.addSignalFence(*fence);
    displaySubmitCommand.setSignalFenceIndexPtr(&state.frameIdx);
    displaySubmitCommand.addDependency(displayRenderCommand);
    displaySubmitCommand.addDependency(displayUpdateInUseCommand);
    displaySubmitCommand.addDependency(*state.guiSubmitCmd);
    displaySubmitCommand.addDependency(*state.viewerSubmitCmd);

    auto& presentCommand = state.commandQueue->createCommand<sol::PresentCommand>();
    presentCommand.setName("(Display)");
    presentCommand.setSwapchain(*state.swapchain);
    for (auto& sem : state.displayRenderFinishedSemaphores) presentCommand.addWaitSemaphore(*sem);
    presentCommand.setImageIndexPtr(&state.imageIdx);
    presentCommand.setWaitSemaphoreIndexPtr(&state.frameIdx);
    presentCommand.addDependency(displaySubmitCommand);
}
