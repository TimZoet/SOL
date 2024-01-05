#include "sol-render-test/graphics/graphics_traverser.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-render/graphics/graphics_render_data.h"
#include "sol-render/graphics/graphics_traverser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "testutils/scenegraph.h"

void GraphicsTraverser::operator()()
{
    const sol::GeometryBufferAllocator::Settings settings{.memoryManager = getMemoryManager(),
                                                          .strategy = sol::GeometryBufferAllocator::Strategy::Separate};
    const auto geometryBufferAllocator = sol::GeometryBufferAllocator::create(settings);
    auto       scenegraph = Scenegraphs::load(Scenegraphs::Graphics::Name::Simple, *geometryBufferAllocator);

    sol::GraphicsTraverser  traverser;
    sol::GraphicsRenderData renderData;
    traverser.setRenderData(&renderData);
    traverser.traverse(scenegraph.scenegraph->getRootNode());


    compareEQ(3, renderData.drawables.size());
    compareEQ(6, renderData.descriptors.size());
    compareEQ(0, renderData.pushConstantRanges.size());
    compareEQ(0, renderData.pushConstantData.size());
    compareEQ(2, renderData.dynamicStates.size());
    compareEQ(6, renderData.dynamicStateReferences.size());
}
