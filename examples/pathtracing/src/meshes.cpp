#include "pathtracing/meshes.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh_description.h"
#include "sol-mesh/mesh_layout.h"
#include "sol-mesh/mesh_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/edit_mesh.h"

sol::MeshLayout& createGuiMeshLayout(sol::MeshManager& manager)
{
    auto& meshLayout = manager.createMeshLayout("imgui");
    meshLayout.addBinding("vertex", 0, 20, VK_VERTEX_INPUT_RATE_VERTEX);
    meshLayout.addAttribute("pos", 0, 0, VK_FORMAT_R32G32_SFLOAT, 0);
    meshLayout.addAttribute("uv", 1, 0, VK_FORMAT_R32G32_SFLOAT, 8);
    meshLayout.addAttribute("col", 2, 0, VK_FORMAT_R8G8B8A8_UNORM, 16);
    meshLayout.finalize();

    return meshLayout;
}

sol::MeshLayout& createViewerMeshLayout(sol::MeshManager& manager)
{
    auto& meshLayout = manager.createMeshLayout("viewer");
    meshLayout.addBinding("vertex", 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
    meshLayout.addAttribute("pos", 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
    meshLayout.addAttribute("normal", 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
    meshLayout.addAttribute("tangent", 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, tangent));
    meshLayout.addAttribute("color", 3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color));
    meshLayout.addAttribute("uv0", 4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, uv0));
    meshLayout.addAttribute("uv1", 5, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, uv1));
    meshLayout.finalize();

    return meshLayout;
}

sol::MeshLayout& createDisplayMeshLayout(sol::MeshManager& manager)
{
    auto& meshLayout = manager.createMeshLayout("display");
    meshLayout.addBinding("vertex", 0, 16, VK_VERTEX_INPUT_RATE_VERTEX);
    meshLayout.addAttribute("pos", 0, 0, VK_FORMAT_R32G32_SFLOAT, 0);
    meshLayout.addAttribute("uv", 1, 0, VK_FORMAT_R32G32_SFLOAT, 8);
    meshLayout.finalize();

    return meshLayout;
}


sol::IndexedMesh& createQuadMesh(sol::MeshManager& manager)
{
    auto quadDescription = manager.createMeshDescription();
    quadDescription->addVertexBuffer(16, 4);
    quadDescription->addIndexBuffer(2, 6);

    const std::array quadVerts = {math::float2(-1, -1),
                                  math::float2(0, 0),
                                  math::float2(1, -1),
                                  math::float2(1, 0),
                                  math::float2(1, 1),
                                  math::float2(1, 1),
                                  math::float2(-1, 1),
                                  math::float2(0, 1)};

    const std::array<int16_t, 6> quadIndices = {0, 1, 2, 2, 3, 0};

    quadDescription->setVertexData(0, 0, 4, quadVerts.data());
    quadDescription->setIndexData(0, 6, quadIndices.data());

    return manager.createIndexedMesh(std::move(quadDescription));
}