#include "sol-scenegraph-test/scenegraph.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/scenegraph.h"

void Scenegraph::operator()()
{
    const auto scenegraph = std::make_unique<sol::Scenegraph>();
    expectNoThrow([&] { static_cast<void>(scenegraph->getRootNode()); });
}
