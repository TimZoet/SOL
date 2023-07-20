#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol-mesh/mesh_manager.h"

namespace sol
{
    // TODO: This whole class should be turned into more of an interface.
    // The exact layout of the data is application dependent, so having a
    // fixed class here does not make much sense.
    // ...or... scratch that? Is the MeshDescription not good enough? This
    // class is perhaps a bit of a redundant layer. File > Memory > EditMesh > MeshDescription > IMesh is overkill.
    class EditMesh
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Vertex
        {
            math::float3 pos;
            math::float3 color;
            math::float2 uv;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        EditMesh() = default;

        EditMesh(const EditMesh&) = delete;

        EditMesh(EditMesh&&) = delete;

        ~EditMesh() = default;

        EditMesh& operator=(const EditMesh&) = delete;

        EditMesh& operator=(EditMesh&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::vector<math::float3>& getPositions() noexcept;

        [[nodiscard]] const std::vector<math::float3>& getPositions() const noexcept;

        [[nodiscard]] std::vector<math::float2>& getUvs() noexcept;

        [[nodiscard]] const std::vector<math::float2>& getUvs() const noexcept;

        [[nodiscard]] std::vector<math::float3>& getColors() noexcept;

        [[nodiscard]] const std::vector<math::float3>& getColors() const noexcept;

        [[nodiscard]] std::vector<math::uint3>& getIndices() noexcept;

        [[nodiscard]] const std::vector<math::uint3>& getIndices() const noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] MeshDescriptionPtr toMeshDescription(MeshManager& meshManager) const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<math::float3> positions;
        std::vector<math::float2> uvs;
        std::vector<math::float3> colors;
        std::vector<math::uint3>  indices;
    };
}  // namespace sol