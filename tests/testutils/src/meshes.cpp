#include "testutils/meshes.h"

namespace
{
    [[nodiscard]] sol::MeshPtr loadDummy(sol::GeometryBufferAllocator& allocator)
    {
        return std::make_unique<sol::Mesh>(allocator.allocateVertexBuffer(256, 16));
    }
}  // namespace

sol::MeshPtr Meshes::load(const Name name, sol::GeometryBufferAllocator& allocator)
{
    switch (name)
    {
    case Name::Dummy: return loadDummy(allocator);
    }

    throw std::runtime_error("");
}
