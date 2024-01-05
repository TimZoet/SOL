#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/mesh.h"
#include "sol-mesh/vertex_buffer.h"

class Meshes
{
public:
    enum class Name
    {
        // A dummy mesh with a small vertex buffer with zeroed out data.
        Dummy
    };

    [[nodiscard]] static sol::MeshPtr load(Name name, sol::GeometryBufferAllocator& allocator);
};
