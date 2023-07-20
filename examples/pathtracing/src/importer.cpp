#include "pathtracing/importer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "OpenImageIO/filesystem.h"
#include "OpenImageIO/imageio.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-material/forward/forward_material_manager.h"
#include "sol-memory/memory_manager.h"
#include "sol-mesh/indexed_mesh.h"
#include "sol-mesh/mesh_description.h"
#include "sol-mesh/mesh_manager.h"
#include "sol/scenegraph/scenegraph.h"
#include "sol/scenegraph/forward/forward_material_node.h"
#include "sol/scenegraph/drawable/mesh_node.h"
#include "sol-texture/texture_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/edit_mesh.h"
#include "pathtracing/meshes.h"
#include "pathtracing/materials/model_transform_node.h"
#include "pathtracing/materials/viewer_attributes_material.h"
#include "pathtracing/materials/viewer_lit_material.h"
#include "pathtracing/materials/viewer_textures_material.h"

namespace
{
    void importNode(const tinygltf::Model&                            model,
                    const tinygltf::Node&                             node,
                    sol::Node&                                        parentNode,
                    sol::ForwardMaterial&                             viewerAttributesMaterial,
                    sol::ForwardMaterial&                             viewerLitMaterial,
                    sol::ForwardMaterial&                             viewerTexturesMaterial,
                    const std::vector<sol::IndexedMesh*>&             meshes,
                    const std::vector<sol::ForwardMaterialInstance*>& materials)
    {
        auto& transformNode = parentNode.addChild(std::make_unique<ModelTransformNode>(viewerAttributesMaterial));

        auto translation = math::zero<math::float3>();
        auto scale       = math::one<math::float3>();
        auto rotation    = math::float4(0, 0, 0, 1);

        if (!node.translation.empty())
            translation = math::float3(node.translation[0], node.translation[1], node.translation[2]);
        if (!node.scale.empty()) scale = math::float3(node.scale[0], node.scale[1], node.scale[2]);
        if (!node.rotation.empty())
            rotation = math::float4(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);

        math::mat4x4f transform =
          math::rotation_from_quat(rotation) * math::scale(scale) * math::translation(translation);

        transformNode.setModel(math::transpose(transform));

        if (node.mesh != -1)
        {
            // TODO:
            auto& mesh              = *meshes[node.mesh];
            auto& mtlAttributes     = *materials[model.meshes[node.mesh].primitives[0].material * 3 + 0];
            auto& mtlLit            = *materials[model.meshes[node.mesh].primitives[0].material * 3 + 1];
            auto& mtlTextures       = *materials[model.meshes[node.mesh].primitives[0].material * 3 + 2];
            auto& mtlAttributesNode = transformNode.addChild(std::make_unique<sol::ForwardMaterialNode>(mtlAttributes));
            auto& mtlLitNode        = transformNode.addChild(std::make_unique<sol::ForwardMaterialNode>(mtlLit));
            auto& mtlTexturesNode   = transformNode.addChild(std::make_unique<sol::ForwardMaterialNode>(mtlTextures));
            mtlAttributesNode.addChild(std::make_unique<sol::MeshNode>(mesh));
            mtlLitNode.addChild(std::make_unique<sol::MeshNode>(mesh));
            mtlTexturesNode.addChild(std::make_unique<sol::MeshNode>(mesh));

            mtlAttributesNode.setGeneralMask(GlobalState::ViewMode::Attributes);
            mtlLitNode.setGeneralMask(GlobalState::ViewMode::Lit);
            mtlTexturesNode.setGeneralMask(GlobalState::ViewMode::Textures);
        }

        for (const auto i : node.children)
            importNode(model,
                       model.nodes[i],
                       transformNode,
                       viewerAttributesMaterial,
                       viewerLitMaterial,
                       viewerTexturesMaterial,
                       meshes,
                       materials);
    }
}  // namespace

bool importScene(GlobalState& state)
{
    auto gltfModel = importGltf(state.importFilename);
    if (!gltfModel) return false;

    state.sceneMeshes    = importMeshes(*gltfModel, *state.viewerMeshManager);
    state.sceneImages    = importImages(state.importFilename, *gltfModel, *state.viewerTextureManager);
    state.sceneTextures  = importTextures(*gltfModel, state.sceneImages, *state.viewerTextureManager);
    state.sceneMaterials = importMaterials(*gltfModel,
                                           state.sceneTextures,
                                           *state.viewerMtlManager,
                                           *state.viewerAttributesMtl,
                                           *state.viewerLitMtl,
                                           *state.viewerTexturesMtl,
                                           *state.viewerMissingTexture);
    importNodes(*gltfModel,
                state.sceneMeshes,
                state.sceneMaterials,
                *state.viewerAttributesMtl,
                *state.viewerLitMtl,
                *state.viewerTexturesMtl,
                *state.viewerRootNode);

    dot::Graph graph;
    state.viewerScenegraph->visualize(graph);
    std::ofstream file("scenegraph.dot");
    graph.write(file);

    return true;
}

std::unique_ptr<tinygltf::Model> importGltf(const std::filesystem::path& path)
{
    auto               model = std::make_unique<tinygltf::Model>();
    tinygltf::TinyGLTF loader;
    std::string        err;
    std::string        warn;

    const bool ret = path.extension() == ".gltf" ? loader.LoadASCIIFromFile(model.get(), &err, &warn, path.string()) :
                                                   loader.LoadBinaryFromFile(model.get(), &err, &warn, path.string());

    if (!warn.empty()) std::cout << "glTF import warning: " << warn << std::endl;
    if (!err.empty()) std::cout << "glTF import error: " << err << std::endl;
    if (!ret)
    {
        std::cout << "Failed to parse glTF" << std::endl;
        return nullptr;
    }

    return model;
}

std::vector<sol::Image2D*>
  importImages(const std::filesystem::path& path, const tinygltf::Model& model, sol::TextureManager& textureManager)
{
    std::vector<sol::Image2D*> images;

    for (const auto& img : model.images)
    {
        OIIO::ImageInput::unique_ptr inp;

        // Load image from disk.
        if (!img.uri.empty())
        {
            const auto f = std::filesystem::path(img.uri);
            if (f.is_relative())
                inp = OIIO::ImageInput::open((path.parent_path() / f).string());
            else
                inp = OIIO::ImageInput::open(f.string());
        }
        // Load image from memory.
        else
        {
            const auto&                   bufferView = model.bufferViews[img.bufferView];
            const auto&                   buffer     = model.buffers[bufferView.buffer];
            OIIO::Filesystem::IOMemReader memreader(
              const_cast<void*>(reinterpret_cast<const void*>(buffer.data.data() + bufferView.byteOffset)),
              bufferView.byteLength);

            if (img.mimeType == "image/png")
                inp = OIIO::ImageInput::open(".png", nullptr, &memreader);
            else if (img.mimeType == "image/jpeg")
                inp = OIIO::ImageInput::open(".jpeg", nullptr, &memreader);
            else
                throw std::runtime_error("Unexpected mimeType " + img.mimeType);
        }

        if (!inp) throw std::runtime_error("");

        // Get image properties.
        const auto& spec     = inp->spec();
        const auto  width    = static_cast<uint32_t>(spec.width);
        const auto  height   = static_cast<uint32_t>(spec.height);
        const auto  channels = spec.nchannels;
        if (channels != 3 && channels != 4) throw std::runtime_error("");

        // Get image data.
        std::vector pixels(static_cast<size_t>(width) * height * 4, 1.0f);
        inp->read_image(0, 0, 0, channels, OIIO::TypeDesc::FLOAT, pixels.data(), sizeof(float) * 4);
        inp->close();

        // Create RGB or RGBA image.
        auto& image = textureManager.createImage2D(
          VK_FORMAT_R32G32B32A32_SFLOAT,
          std::array{static_cast<uint32_t>(spec.width), static_cast<uint32_t>(spec.height)},
          pixels.data());
        images.emplace_back(&image);
    }

    return images;
}

std::vector<sol::Texture2D*> importTextures(const tinygltf::Model&            model,
                                            const std::vector<sol::Image2D*>& images,
                                            sol::TextureManager&              textureManager)
{
    std::vector<sol::Texture2D*> textures;

    for (const auto& tex : model.textures)
    {
        // TODO: Samplers.
        auto&                        image = *images.at(tex.source);
        sol::VulkanSampler::Settings settings;
        settings.device       = textureManager.getMemoryManager().getDevice();
        settings.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        settings.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        auto& texture         = textureManager.createTexture2D(image, nullptr, sol::VulkanSampler::create(settings));
        textures.emplace_back(&texture);
    }

    return textures;
}

std::vector<sol::ForwardMaterialInstance*> importMaterials(const tinygltf::Model&              model,
                                                           const std::vector<sol::Texture2D*>& textures,
                                                           sol::ForwardMaterialManager&        materialManager,
                                                           sol::ForwardMaterial&               viewerAttributesMaterial,
                                                           sol::ForwardMaterial&               viewerLitMaterial,
                                                           sol::ForwardMaterial&               viewerTexturesMaterial,
                                                           sol::Texture2D&                     missingTexture)
{
    std::vector<sol::ForwardMaterialInstance*> materials;

    for (const auto& mtl : model.materials)
    {
        sol::Texture2D* texDiffuse           = &missingTexture;
        sol::Texture2D* texNormal            = &missingTexture;
        sol::Texture2D* texMetallicRoughness = &missingTexture;
        sol::Texture2D* texOcclusion         = &missingTexture;
        sol::Texture2D* texEmissive          = &missingTexture;
        // TODO: Multiple UV coordinates.
        // TODO: Factors.
        if (mtl.pbrMetallicRoughness.baseColorTexture.index != -1)
            texDiffuse = textures.at(mtl.pbrMetallicRoughness.baseColorTexture.index);
        if (mtl.normalTexture.index != -1) texNormal = textures.at(mtl.normalTexture.index);
        if (mtl.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
            texMetallicRoughness = textures.at(mtl.pbrMetallicRoughness.metallicRoughnessTexture.index);
        if (mtl.occlusionTexture.index != -1) texOcclusion = textures.at(mtl.occlusionTexture.index);
        if (mtl.emissiveTexture.index != -1) texEmissive = textures.at(mtl.emissiveTexture.index);

        materials.emplace_back(&materialManager.addMaterialInstance(
          viewerAttributesMaterial, std::make_unique<ViewerAttributesMaterialInstance>()));
        materials.emplace_back(&materialManager.addMaterialInstance(
          viewerLitMaterial,
          std::make_unique<ViewerLitMaterialInstance>(
            *texDiffuse, *texNormal, *texMetallicRoughness, *texOcclusion, *texEmissive)));
        materials.emplace_back(&materialManager.addMaterialInstance(
          viewerTexturesMaterial,
          std::make_unique<ViewerTexturesMaterialInstance>(
            *texDiffuse, *texNormal, *texMetallicRoughness, *texOcclusion, *texEmissive)));
    }

    return materials;
}

std::vector<sol::IndexedMesh*> importMeshes(const tinygltf::Model& model, sol::MeshManager& meshManager)
{
    std::vector<sol::IndexedMesh*> meshes;

    for (const auto& mesh : model.meshes)
    {
        // TODO: Handle multiple primitives.
        const auto& prim = mesh.primitives.front();
        if (prim.mode != TINYGLTF_MODE_TRIANGLES) throw std::runtime_error("");

        auto desc = std::make_unique<sol::MeshDescription>(meshManager);

        std::vector<Vertex>   vertexData;
        std::vector<uint32_t> indexData;

        if (prim.attributes.contains("POSITION"))
        {
            const auto  accessor   = model.accessors[prim.attributes.at("POSITION")];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer     = model.buffers[bufferView.buffer];
            vertexData.resize(accessor.count);
            const auto* data = reinterpret_cast<const math::float3*>(buffer.data.data() + bufferView.byteOffset);
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].pos = *(data + i);
        }

        if (prim.attributes.contains("NORMAL"))
        {
            const auto  accessor   = model.accessors[prim.attributes.at("NORMAL")];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer     = model.buffers[bufferView.buffer];
            const auto* data       = reinterpret_cast<const math::float3*>(buffer.data.data() + bufferView.byteOffset);
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].normal = *(data + i);
        }
        else
        {
            // TODO: Calculate normals.
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].normal = math::float3(0, 0, 1);
        }

        if (prim.attributes.contains("TANGENT"))
        {
            const auto  accessor   = model.accessors[prim.attributes.at("TANGENT")];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer     = model.buffers[bufferView.buffer];
            const auto* data       = reinterpret_cast<const math::float4*>(buffer.data.data() + bufferView.byteOffset);
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].tangent = *(data + i);
        }
        else
        {
            // TODO: Calculate tangents.
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].tangent = math::float4(1, 0, 0, 1);
        }

        if (prim.attributes.contains("COLOR"))
        {
            const auto  accessor   = model.accessors[prim.attributes.at("COLOR")];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer     = model.buffers[bufferView.buffer];
            const auto* data       = reinterpret_cast<const math::float4*>(buffer.data.data() + bufferView.byteOffset);
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].color = *(data + i);
        }
        else
        {
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].color = math::one<math::float4>();
        }

        if (prim.attributes.contains("TEXCOORD_0"))
        {
            const auto  accessor   = model.accessors[prim.attributes.at("TEXCOORD_0")];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer     = model.buffers[bufferView.buffer];

            const auto* data = reinterpret_cast<const math::float2*>(buffer.data.data() + bufferView.byteOffset);
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].uv0 = *(data + i);
        }
        else
        {
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].uv0 = math::zero<math::float2>();
        }

        if (prim.attributes.contains("TEXCOORD_1"))
        {
            const auto  accessor   = model.accessors[prim.attributes.at("TEXCOORD_1")];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer     = model.buffers[bufferView.buffer];

            const auto* data = reinterpret_cast<const math::float2*>(buffer.data.data() + bufferView.byteOffset);
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].uv1 = *(data + i);
        }
        else
        {
            for (size_t i = 0; i < vertexData.size(); i++) vertexData[i].uv1 = math::zero<math::float2>();
        }

        {
            const auto  accessor   = model.accessors[prim.indices];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer     = model.buffers[bufferView.buffer];

            indexData.resize(accessor.count);

            if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                auto* data = buffer.data.data() + bufferView.byteOffset;
                for (size_t i = 0; i < accessor.count; i++)
                {
                    indexData[i] = *reinterpret_cast<const uint16_t*>(data);
                    data += 2;
                }
            }
            else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                std::memcpy(indexData.data(), buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
            }
        }

        desc->addVertexBuffer(sizeof(Vertex), static_cast<uint32_t>(vertexData.size()));
        desc->setVertexData(0, 0, vertexData.size(), vertexData.data());

        desc->addIndexBuffer(sizeof(uint32_t), static_cast<uint32_t>(indexData.size()));
        desc->setIndexData(0, indexData.size(), indexData.data());

        auto& indexedMesh = meshManager.createIndexedMesh(std::move(desc));
        meshes.emplace_back(&indexedMesh);
    }

    return meshes;
}

void importNodes(const tinygltf::Model&                            model,
                 const std::vector<sol::IndexedMesh*>&             meshes,
                 const std::vector<sol::ForwardMaterialInstance*>& materials,
                 sol::ForwardMaterial&                             viewerAttributesMaterial,
                 sol::ForwardMaterial&                             viewerLitMaterial,
                 sol::ForwardMaterial&                             viewerTexturesMaterial,
                 sol::Node&                                        rootNode)
{
    // Import nodes of default scene.
    if (model.defaultScene != -1)
    {
        for (const auto node : model.scenes[model.defaultScene].nodes)
            importNode(model,
                       model.nodes[node],
                       rootNode,
                       viewerAttributesMaterial,
                       viewerLitMaterial,
                       viewerTexturesMaterial,
                       meshes,
                       materials);
    }
    // Import nodes of first scene.
    else if (!model.scenes.empty())
    {
        for (const auto node : model.scenes.front().nodes)
            importNode(model,
                       model.nodes[node],
                       rootNode,
                       viewerAttributesMaterial,
                       viewerLitMaterial,
                       viewerTexturesMaterial,
                       meshes,
                       materials);
    }
    else { throw std::runtime_error(""); }
}