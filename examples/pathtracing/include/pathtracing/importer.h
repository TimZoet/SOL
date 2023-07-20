#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "tinygltf/tiny_gltf.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"
#include "sol-mesh/fwd.h"
#include "sol-scenegraph/node.h"
#include "sol-texture/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/global_state.h"

bool importScene(GlobalState& state);

std::unique_ptr<tinygltf::Model> importGltf(const std::filesystem::path& path);

std::vector<sol::Image2D*>
  importImages(const std::filesystem::path& path, const tinygltf::Model& model, sol::TextureManager& textureManager);

std::vector<sol::Texture2D*> importTextures(const tinygltf::Model&            model,
                                            const std::vector<sol::Image2D*>& images,
                                            sol::TextureManager&              textureManager);

std::vector<sol::ForwardMaterialInstance*> importMaterials(const tinygltf::Model&              model,
                                                           const std::vector<sol::Texture2D*>& textures,
                                                           sol::ForwardMaterialManager&        materialManager,
                                                           sol::ForwardMaterial&               viewerAttributesMaterial,
                                                           sol::ForwardMaterial&               viewerLitMaterial,
                                                           sol::ForwardMaterial&               viewerTexturesMaterial,
                                                           sol::Texture2D&                     missingTexture);

std::vector<sol::IndexedMesh*> importMeshes(const tinygltf::Model& model, sol::MeshManager& meshManager);

void importNodes(const tinygltf::Model&                            model,
                 const std::vector<sol::IndexedMesh*>&             meshes,
                 const std::vector<sol::ForwardMaterialInstance*>& materials,
                 sol::ForwardMaterial&                             viewerAttributesMaterial,
                 sol::ForwardMaterial&                             viewerLitMaterial,
                 sol::ForwardMaterial&                             viewerTexturesMaterial,
                 sol::Node&                                        rootNode);
