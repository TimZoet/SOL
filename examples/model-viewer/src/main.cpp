////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "model-viewer/model_viewer_application.h"

int main(int argc, char** argv)
{
    PathtracingApplication app;
    if (!app.parse(argc, argv)) return 0;
    app.initialize();
    app.run();
    return 0;
}
