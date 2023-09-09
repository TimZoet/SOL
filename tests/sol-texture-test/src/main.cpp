////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture-test/image/image2d.h"
#include "sol-texture-test/image/image2d_barriers.h"
#include "sol-texture-test/image/image2d_data.h"
#include "sol-texture-test/sampler/sampler2d.h"

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
    // TODO: Parallel tests are not supported. BetterTest needs an option to always disable them and perhaps even give an error when trying run in parallel.
    return bt::run<Image2D, Image2DBarriers, Image2DData, Sampler2D>(argc, argv, "sol-texture");
}
