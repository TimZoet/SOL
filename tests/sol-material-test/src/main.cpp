
////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material-test/material.h"
#include "sol-material-test/material_instance.h"
#include "sol-material-test/graphics/graphics_dynamic_state.h"
#include "sol-material-test/graphics/graphics_material.h"
#include "sol-material-test/graphics/graphics_material_instance.h"

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

    return bt::run<Material, MaterialInstance, GraphicsDynamicState, GraphicsMaterial, GraphicsMaterialInstance>(
      argc, argv, "sol-material");
}
