#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh-test/geometry_buffer_allocator.h"
#include "sol-mesh-test/index_buffer.h"
#include "sol-mesh-test/mesh.h"
#include "sol-mesh-test/vertex_buffer.h"

#ifdef WIN32
#include "Windows.h"
#endif

int main(int argc, char** argv)
{
    // Set path next to executable.
#ifdef WIN32
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const std::filesystem::path workdir(path);
        const auto                  p = workdir.parent_path();
        std::filesystem::current_path(p);
    }
#endif

    return bt::run<GeometryBufferAllocator, IndexBuffer, Mesh, VertexBuffer>(argc, argv, "sol-mesh");
}
