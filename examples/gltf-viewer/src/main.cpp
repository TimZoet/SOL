
////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "gltf-viewer/application.h"

int main(int argc, char** argv)
{
    Application app;
    if (!app.parse(argc, argv)) return 1;
    app.initialize();
    app.run();
    return 0;
}