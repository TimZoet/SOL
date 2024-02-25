
////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph-test/node.h"
#include "sol-scenegraph-test/scenegraph.h"
#include "sol-scenegraph-test/drawable/mesh_node.h"
#include "sol-scenegraph-test/graphics/graphics_dynamic_state_node.h"
#include "sol-scenegraph-test/graphics/graphics_material_node.h"
#include "sol-scenegraph-test/graphics/graphics_push_constant_node.h"

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

    return bt::
      run<Node, Scenegraph, MeshNode, GraphicsDynamicStateNode, GraphicsMaterialNode, GraphicsPushConstantNode>(
        argc, argv, "sol-scenegraph");
}
