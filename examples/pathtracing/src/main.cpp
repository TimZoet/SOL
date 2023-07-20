////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/pathtracing_application.h"

int main(int argc, char** argv)
{
    PathtracingApplication app;
    if (!app.parse(argc, argv)) return 0;
    app.initialize();
    app.run();
    return 0;
}
