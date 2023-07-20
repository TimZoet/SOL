#include "pathtracing/scenegraphs.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/indexed_mesh.h"
#include "sol-scenegraph/scenegraph.h"
#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/forward/forward_material_node.h"

void createGuiScenegraph(GlobalState& state)
{
    state.guiMaterialNode =
      &state.guiScenegraph->getRootNode().addChild(std::make_unique<sol::ForwardMaterialNode>(*state.guiMtlInstance));
    state.guiTransformNode = &state.guiMaterialNode->addChild(std::make_unique<GuiTransformNode>(*state.guiMtl));
}

void createViewerScenegraph(GlobalState& state)
{
    state.viewerRootNode = &state.viewerScenegraph->getRootNode().addChild(
      std::make_unique<sol::ForwardMaterialNode>(*state.viewerMtlInstance));
}

void createDisplayScenegraph(GlobalState& state)
{
    auto& displayNode0 = state.displayScenegraph->getRootNode().addChild(
      std::make_unique<sol::ForwardMaterialNode>(*state.displayMtlInstanceViewer));
    auto& displayNode1 = state.displayScenegraph->getRootNode().addChild(
      std::make_unique<sol::ForwardMaterialNode>(*state.displayMtlInstanceGui));
    displayNode0.addChild(std::make_unique<sol::MeshNode>(*state.quadMesh));
    displayNode1.addChild(std::make_unique<sol::MeshNode>(*state.quadMesh));
}