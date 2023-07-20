#include "pathtracing/imgui_context.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "luna/window.h"
#include "sol-core/vulkan_device.h"
#include "sol-mesh/indexed_mesh.h"
#include "sol-mesh/mesh_description.h"
#include "sol-mesh/mesh_manager.h"
#include "sol-mesh/shared_mesh.h"
#include "sol-render/forward/forward_traverser.h"
#include "sol/scenegraph/drawable/mesh_node.h"
#include "sol/scenegraph/forward/forward_material_node.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/imgui_impl_glfw.h"
#include "pathtracing/imgui_stdlib.h"
#include "pathtracing/materials/gui_material.h"

void createImguiContext(GlobalState& state)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    state.imgui                      = &ImGui::GetIO();
    state.imgui->DisplaySize.x       = static_cast<float>(state.width);
    state.imgui->DisplaySize.y       = static_cast<float>(state.height);
    state.imgui->BackendRendererName = "sol";
    state.imgui->BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOther(state.window->get(), true);
}

void destroyImguiContext() { ImGui::DestroyContext(); }

void imguiFrame(GlobalState& state)
{
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({512, 128});
    ImGui::Begin("Load Model");
    ImGui::InputText("File", &state.importFilename);
    state.importButtonPressed = ImGui::Button("Load Model");
    ImGui::End();

    ImGui::Begin("Display");
    state.updateViewMode = ImGui::RadioButton("Attributes",
                                              reinterpret_cast<int32_t*>(&state.viewMode),
                                              static_cast<int32_t>(GlobalState::ViewMode::Attributes));
    ImGui::SameLine();
    state.updateViewMode |= ImGui::RadioButton(
      "Lit", reinterpret_cast<int32_t*>(&state.viewMode), static_cast<int32_t>(GlobalState::ViewMode::Lit));
    ImGui::SameLine();
    state.updateViewMode |= ImGui::RadioButton(
      "Textures", reinterpret_cast<int32_t*>(&state.viewMode), static_cast<int32_t>(GlobalState::ViewMode::Textures));

    if (state.viewMode == GlobalState::ViewMode::Attributes)
    {
        ImGui::Combo("Show Attribute", &state.activeAttribute, "Normal\0Tangent\0Color\0UV0\0UV1\0\0");
    }
    else if (state.viewMode == GlobalState::ViewMode::Lit) {}
    else if (state.viewMode == GlobalState::ViewMode::Textures)
    {
        ImGui::Combo("Show Texture",
                     &state.activeTexture,
                     "Diffuse\0Normal\0Metallic\0Roughness\0Occlusion\0Emissive\0Opacity\0\0");
    }

    if (state.updateViewMode)
    {
        state.viewerTraverser->setGeneralMaskFunction([&state](const uint64_t mask) {
            if (mask == 0 || mask & state.viewMode) return sol::ITraverser::TraversalAction::Continue;
            return sol::ITraverser::TraversalAction::Terminate;
        });
    }

    ImGui::End();

    ImGui::Render();
}

void imguiUpdate(GlobalState& state)
{
    for (const auto& id : state.oldMeshes) state.guiMeshManager->destroyMesh(id);
    state.oldMeshes.clear();
    state.guiTransformNode->clearChildren();

    const auto* drawData = ImGui::GetDrawData();
    if (drawData->TotalVtxCount == 0) return;

    math::float2 scale;
    scale[0] = 2.0f / drawData->DisplaySize.x;
    scale[1] = 2.0f / drawData->DisplaySize.y;
    math::float2 translate;
    translate[0] = -1.0f - drawData->DisplayPos.x * scale[0];
    translate[1] = -1.0f - drawData->DisplayPos.y * scale[1];
    state.guiTransformNode->setScale(scale);
    state.guiTransformNode->setTranslation(translate);
    vkDeviceWaitIdle(state.device->get());

    auto desc = state.guiMeshManager->createMeshDescription();
    desc->addVertexBuffer(sizeof(ImDrawVert), drawData->TotalVtxCount);
    desc->addIndexBuffer(sizeof(ImDrawIdx), drawData->TotalIdxCount);
    uint32_t idxOffset = 0, vtxOffset = 0;
    for (int i = 0; i < drawData->CmdListsCount; i++)
    {
        const auto* cmdList = drawData->CmdLists[i];

        desc->setIndexData(idxOffset, cmdList->IdxBuffer.Size, cmdList->IdxBuffer.Data);
        desc->setVertexData(0, vtxOffset, cmdList->VtxBuffer.Size, cmdList->VtxBuffer.Data);

        idxOffset += cmdList->IdxBuffer.Size;
        vtxOffset += cmdList->VtxBuffer.Size;
    }

    auto& meshInstance = state.guiMeshManager->createIndexedMesh(std::move(desc));
    state.oldMeshes.emplace_back(meshInstance.getUuid());

    idxOffset = 0;
    vtxOffset = 0;
    for (int i = 0; i < drawData->CmdListsCount; i++)
    {
        const auto* cmdList = drawData->CmdLists[i];

        for (int j = 0; j < cmdList->CmdBuffer.Size; j++)
        {
            const auto& cmd = cmdList->CmdBuffer[j];

            auto& mtlNode = state.guiTransformNode->addChild(
              std::make_unique<sol::ForwardMaterialNode>(*static_cast<GuiMaterialInstance*>(cmd.TextureId)));

            auto& sharedMesh = state.guiMeshManager->createSharedMesh();
            state.oldMeshes.emplace_back(sharedMesh.getUuid());
            sharedMesh.addVertexBuffer(meshInstance);
            sharedMesh.setVertexOffset(vtxOffset + cmd.VtxOffset);
            sharedMesh.setIndexBuffer(&meshInstance);
            sharedMesh.setIndexCount(cmd.ElemCount);
            sharedMesh.setFirstIndex(idxOffset + cmd.IdxOffset);

            mtlNode.addChild(std::make_unique<sol::MeshNode>(sharedMesh));
        }

        idxOffset += cmdList->IdxBuffer.Size;
        vtxOffset += cmdList->VtxBuffer.Size;
    }
}
