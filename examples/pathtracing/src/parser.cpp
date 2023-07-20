#include "pathtracing/parser.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "parsertongue/parser.h"

Parser::Parser(int argc, char** argv)
{
    ////////////////////////////////////////////////////////////////
    // Create parser.
    ////////////////////////////////////////////////////////////////

    auto parser = pt::parser(argc, argv);
    parser.set_name("Forward Renderer");
    parser.set_version("");
    parser.set_description("");

    const auto vWidth = parser.add_value<uint32_t>('\0', "width");
    vWidth->set_default(1280);
    const auto vHeight = parser.add_value<uint32_t>('\0', "height");
    vHeight->set_default(720);

    const auto vShaderCache = parser.add_value<std::filesystem::path>('\0', "shadercache");
    vShaderCache->set_default("shadercache.db");

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    // Run parser.
    std::string e;
    if (!parser(e))
    {
        std::cerr << "Internal parsing error: " << e << std::endl;
        return;
    }

    // Display help and errors.
    if (parser.display_help(std::cout)) return;

    if (!parser.get_errors().empty())
    {
        parser.display_errors(std::cerr);
        return;
    }

    if (parser.get_operands().empty())
    {
        std::cerr << "Path to model file is required." << std::endl;
        return;
    }

    ////////////////////////////////////////////////////////////////
    // Process.
    ////////////////////////////////////////////////////////////////

    width       = vWidth->get_value();
    height      = vHeight->get_value();
    shaderCache = vShaderCache->get_value();
    modelFile   = parser.get_operands().front();

    valid = true;
}
