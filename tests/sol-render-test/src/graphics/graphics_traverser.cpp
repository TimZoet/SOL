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


    compareEQ(3, renderData.drawables.size()).fatal("Incorrect number of drawables.");
    compareEQ(6, renderData.descriptors.size()).fatal("Incorrect number of descriptors.");
    compareEQ(0, renderData.pushConstantRanges.size()).fatal("Incorrect number of push constant ranges.");
    compareEQ(0, renderData.pushConstantData.size()).fatal("Incorrect number of bytes push constant data.");
    compareEQ(2, renderData.dynamicStates.size()).fatal("Incorrect number of dynamic states.");
    compareEQ(6, renderData.dynamicStateReferences.size()).fatal("Incorrect number of dynamic state references.");

    compareEQ(scenegraph.meshes[0].get(), renderData.drawables[0].mesh);
    compareEQ(scenegraph.meshes[1].get(), renderData.drawables[1].mesh);
    compareEQ(scenegraph.meshes[1].get(), renderData.drawables[2].mesh);

    compareEQ(&scenegraph.materialInstances[0]->operator[](0), renderData.descriptors[0]);
    compareEQ(&scenegraph.materialInstances[1]->operator[](1), renderData.descriptors[1]);
    compareEQ(&scenegraph.materialInstances[0]->operator[](0), renderData.descriptors[2]);
    compareEQ(&scenegraph.materialInstances[2]->operator[](1), renderData.descriptors[3]);
    compareEQ(&scenegraph.materialInstances[5]->operator[](0), renderData.descriptors[4]);
    compareEQ(&scenegraph.materialInstances[5]->operator[](1), renderData.descriptors[5]);

    // Traversing again should be possible and append to the render data.
    traverser.traverse(scenegraph.scenegraph->getRootNode());
    compareEQ(6, renderData.drawables.size());
}
