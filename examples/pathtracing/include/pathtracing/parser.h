#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

struct Parser
{
    Parser(int argc, char** argv);

    bool                  valid  = false;
    uint32_t              width  = -1;
    uint32_t              height = -1;
    std::filesystem::path shaderCache;
    std::filesystem::path modelFile;
};