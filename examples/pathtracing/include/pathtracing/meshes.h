#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol-mesh/fwd.h"

struct Vertex
{
    math::float3 pos;
    math::float3 normal;
    math::float4 tangent;
    math::float4 color;
    math::float2 uv0;
    math::float2 uv1;
};

sol::MeshLayout& createGuiMeshLayout(sol::MeshManager& manager);

sol::MeshLayout& createViewerMeshLayout(sol::MeshManager& manager);

sol::MeshLayout& createDisplayMeshLayout(sol::MeshManager& manager);

/**
 * \brief Create a screen filling quad mesh.
 * \param manager MeshManager.
 * \return Mesh.
 */
sol::IndexedMesh& createQuadMesh(sol::MeshManager& manager);
