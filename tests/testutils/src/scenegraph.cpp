#include "testutils/scenegraph.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"

#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/graphics/graphics_dynamic_state_node.h"
#include "sol-scenegraph/graphics/graphics_material_node.h"

namespace
{
    [[nodiscard]] Scenegraphs::Graphics loadGraphicsSimple(sol::GeometryBufferAllocator& allocator)
    {
        Scenegraphs::Graphics wrapper;

        {
            const sol::DescriptorBuffer::Settings settings{.memoryManager = &allocator.getMemoryManager(),
                                                           .size          = 1024ull * 1024ull};
            wrapper.descriptorBuffer = sol::DescriptorBuffer::create(settings);
        }

        wrapper.materials.emplace_back(Materials::load(Materials::Graphics::Name::Simple, allocator.getDevice()));
        wrapper.materials.emplace_back(Materials::load(Materials::Graphics::Name::Simple, allocator.getDevice()));

        wrapper.materialInstances.emplace_back(wrapper.materials[0].material->createInstance());
        wrapper.materialInstances.back()->enableDescriptor(0, *wrapper.descriptorBuffer);
        wrapper.materialInstances.emplace_back(wrapper.materials[0].material->createInstance());
        wrapper.materialInstances.back()->enableDescriptor(1, *wrapper.descriptorBuffer);
        wrapper.materialInstances.emplace_back(wrapper.materials[0].material->createInstance());
        wrapper.materialInstances.back()->enableDescriptor(1, *wrapper.descriptorBuffer);

        wrapper.materialInstances.emplace_back(wrapper.materials[1].material->createInstance());
        wrapper.materialInstances.back()->enableDescriptor(0, *wrapper.descriptorBuffer);
        wrapper.materialInstances.emplace_back(wrapper.materials[1].material->createInstance());
        wrapper.materialInstances.back()->enableDescriptor(0, *wrapper.descriptorBuffer);
        wrapper.materialInstances.emplace_back(wrapper.materials[1].material->createInstance());
        wrapper.materialInstances.back()->enableDescriptor(0, *wrapper.descriptorBuffer);
        wrapper.materialInstances.back()->enableDescriptor(1, *wrapper.descriptorBuffer);

        wrapper.meshes.emplace_back(Meshes::load(Meshes::Name::Dummy, allocator));
        wrapper.meshes.emplace_back(Meshes::load(Meshes::Name::Dummy, allocator));

        wrapper.scenegraph = std::make_unique<sol::Scenegraph>();
        auto& root         = wrapper.scenegraph->getRootNode();
        auto& dynState     = root.addChild(std::make_unique<sol::GraphicsDynamicStateNode>());
        dynState.getStates().emplace_back(std::make_unique<sol::Scissor>());
        dynState.getStates().emplace_back(std::make_unique<sol::Viewport>());
        {
            auto& mtl0 = dynState.addChild(std::make_unique<sol::GraphicsMaterialNode>(*wrapper.materialInstances[0]));
            auto& mtl1 = mtl0.addChild(std::make_unique<sol::GraphicsMaterialNode>(*wrapper.materialInstances[1]));
            mtl0.addChild(std::make_unique<sol::MeshNode>(*wrapper.meshes[0]));
            auto& mtl2 = mtl0.addChild(std::make_unique<sol::GraphicsMaterialNode>(*wrapper.materialInstances[2]));
            mtl1.addChild(std::make_unique<sol::MeshNode>(*wrapper.meshes[0]));
            mtl2.addChild(std::make_unique<sol::MeshNode>(*wrapper.meshes[1]));
        }
        {
            auto& mtl0 = dynState.addChild(std::make_unique<sol::GraphicsMaterialNode>(*wrapper.materialInstances[3]));
            auto& mtl1 = mtl0.addChild(std::make_unique<sol::GraphicsMaterialNode>(*wrapper.materialInstances[4]));
            auto& mtl2 = mtl0.addChild(std::make_unique<sol::GraphicsMaterialNode>(*wrapper.materialInstances[5]));
            mtl1.addChild(std::make_unique<sol::MeshNode>(*wrapper.meshes[0]));
            mtl2.addChild(std::make_unique<sol::MeshNode>(*wrapper.meshes[1]));
        }

        return wrapper;
    }
}  // namespace

Scenegraphs::Graphics Scenegraphs::load(const Graphics::Name name, sol::GeometryBufferAllocator& allocator)
{
    switch (name)
    {
    case Graphics::Name::Simple: return loadGraphicsSimple(allocator);
    }

    throw std::runtime_error("");
}
