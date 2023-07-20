#include "pathtracing/materials.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "luna/math/matrix.h"
#include "sol-material/forward/forward_material_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/materials/camera_material.h"
#include "pathtracing/materials/gui_material.h"
#include "pathtracing/materials/viewer_attributes_material.h"
#include "pathtracing/materials/viewer_lit_material.h"
#include "pathtracing/materials/viewer_textures_material.h"

void createGuiMaterials(GlobalState& state)
{
    auto guiVertShader = state.shaderCache->getModule("vertex/gui")->createVulkanShaderModuleShared(*state.device);
    auto guiFragShader = state.shaderCache->getModule("fragment/gui")->createVulkanShaderModuleShared(*state.device);

    state.guiMtl = &state.guiMtlManager->addMaterial(std::make_unique<GuiMaterial>(guiVertShader, guiFragShader));
    state.guiMtl->setMeshLayout(*state.guiMeshLayout);
    state.guiMtlInstance = &state.guiMtlManager->addMaterialInstance(
      *state.guiMtl, std::make_unique<GuiMaterialInstance>(*state.guiFontTexture));
}

void createViewerMaterials(GlobalState& state)
{
    auto viewerVertShader =
      state.shaderCache->getModule("vertex/viewer")->createVulkanShaderModuleShared(*state.device);
    auto viewerAttributesFragShader =
      state.shaderCache->getModule("fragment/viewer_attributes")->createVulkanShaderModuleShared(*state.device);
    auto viewerLitFragShader =
      state.shaderCache->getModule("fragment/viewer_lit")->createVulkanShaderModuleShared(*state.device);
    auto viewerTexturesFragShader =
      state.shaderCache->getModule("fragment/viewer_textures")->createVulkanShaderModuleShared(*state.device);

    state.viewerAttributesMtl = &state.viewerMtlManager->addMaterial(
      std::make_unique<ViewerAttributesMaterial>(viewerVertShader, viewerAttributesFragShader));
    state.viewerAttributesMtl->setMeshLayout(*state.viewerMeshLayout);
    state.viewerLitMtl =
      &state.viewerMtlManager->addMaterial(std::make_unique<ViewerLitMaterial>(viewerVertShader, viewerLitFragShader));
    state.viewerLitMtl->setMeshLayout(*state.viewerMeshLayout);
    state.viewerTexturesMtl = &state.viewerMtlManager->addMaterial(
      std::make_unique<ViewerTexturesMaterial>(viewerVertShader, viewerTexturesFragShader));
    state.viewerTexturesMtl->setMeshLayout(*state.viewerMeshLayout);
    state.viewerMtlInstance = &state.viewerMtlManager->addMaterialInstance(*state.viewerAttributesMtl,
                                                                           std::make_unique<CameraMaterialInstance>());

    state.viewerMtlInstance->setProjection(sol::perspectiveMatrix(
      45.0f * (math::m_pi / 180.0f), static_cast<float>(state.width) / static_cast<float>(state.height), 0.1f, 100.0f));
}

void createDisplayMaterials(GlobalState& state)
{
    auto displayVertShader =
      state.shaderCache->getModule("vertex/display")->createVulkanShaderModuleShared(*state.device);
    auto displayFragShader =
      state.shaderCache->getModule("fragment/display")->createVulkanShaderModuleShared(*state.device);

    auto& displayMtl =
      state.displayMtlManager->addMaterial(std::make_unique<DisplayMaterial>(displayVertShader, displayFragShader));
    displayMtl.setMeshLayout(*state.displayMeshLayout);
    state.displayMtlInstanceGui = &state.displayMtlManager->addMaterialInstance(
      displayMtl, std::make_unique<DisplayMaterialInstance>(*state.guiRenderTextures.front()));
    state.displayMtlInstanceViewer = &state.displayMtlManager->addMaterialInstance(
      displayMtl, std::make_unique<DisplayMaterialInstance>(*state.viewerRenderTextures.front()));
}
