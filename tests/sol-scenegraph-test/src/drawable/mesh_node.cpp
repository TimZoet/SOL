#include "sol-scenegraph-test/drawable/mesh_node.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid_system_generator.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh.h"
#include "sol-mesh/vertex_buffer.h"
#include "sol-scenegraph/drawable/mesh_node.h"

void MeshNode::operator()()
{
    const auto mesh = std::make_unique<sol::Mesh>(nullptr);

    /*
     * Test all constructors.
     */

    {
        const auto node = std::make_unique<sol::MeshNode>();
        compareNE(uuids::uuid{}, node->getUuid());
        compareEQ(nullptr, node->getMesh());
    }

    {
        const auto id   = uuids::uuid_system_generator{}();
        const auto node = std::make_unique<sol::MeshNode>(id);
        compareEQ(id, node->getUuid());
        compareEQ(nullptr, node->getMesh());
    }

    {
        const auto id   = uuids::uuid_system_generator{}();
        const auto node = std::make_unique<sol::MeshNode>(id, *mesh);
        compareEQ(id, node->getUuid());
        compareEQ(mesh.get(), node->getMesh());
    }

    {
        const auto node = std::make_unique<sol::MeshNode>(*mesh);
        compareNE(uuids::uuid{}, node->getUuid());
        compareEQ(mesh.get(), node->getMesh());
    }

    /*
     * Test remaining methods.
     */

    {
        const auto node = std::make_unique<sol::MeshNode>();
        compareEQ(sol::Node::Type::Mesh, node->getType());
        compareEQ(nullptr, node->getMesh());
        expectNoThrow([&] { node->setMesh(mesh.get()); });
        compareEQ(mesh.get(), node->getMesh());
    }
}
