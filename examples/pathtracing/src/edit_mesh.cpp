#include "pathtracing/edit_mesh.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh_description.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    std::vector<math::float3>& EditMesh::getPositions() noexcept { return positions; }

    const std::vector<math::float3>& EditMesh::getPositions() const noexcept { return positions; }

    std::vector<math::float2>& EditMesh::getUvs() noexcept { return uvs; }

    const std::vector<math::float2>& EditMesh::getUvs() const noexcept { return uvs; }

    std::vector<math::float3>& EditMesh::getColors() noexcept { return colors; }

    const std::vector<math::float3>& EditMesh::getColors() const noexcept { return colors; }

    std::vector<math::uint3>& EditMesh::getIndices() noexcept { return indices; }

    const std::vector<math::uint3>& EditMesh::getIndices() const noexcept { return indices; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    MeshDescriptionPtr EditMesh::toMeshDescription(MeshManager& meshManager) const
    {
        auto desc = std::make_unique<MeshDescription>(meshManager);
        desc->addVertexBuffer(sizeof(Vertex), positions.size());
        desc->addIndexBuffer(sizeof(uint32_t), indices.size() * 3);

        for (size_t i = 0; i < positions.size(); i++)
        {
            Vertex v{.pos = positions[i], .color = colors[i], .uv = uvs[i]};
            desc->setVertexData(0, i, &v);
        }

        desc->setIndexData(0, indices.size() * 3, indices.data());

        return desc;
    }
}  // namespace sol