#include "sol-mesh/flat_mesh.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh_description.h"
#include "sol-mesh/mesh_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    FlatMesh::FlatMesh() = default;

    FlatMesh::FlatMesh(MeshManager& manager, const uuids::uuid id) : IMesh(manager, id) {}

    FlatMesh::FlatMesh(MeshManager& manager, const uuids::uuid id, VulkanBufferPtr vtxBuffer, const uint32_t vtxCount) :
        IMesh(manager, id), vertexBuffer(std::move(vtxBuffer)), vertexCount(vtxCount)
    {
    }

    FlatMesh::~FlatMesh() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMesh::MeshType FlatMesh::getType() const noexcept { return MeshType::Flat; }

    bool FlatMesh::isValid() const noexcept { return vertexBuffer && vertexCount > 0; }

    bool FlatMesh::isIndexed() const noexcept { return false; }

    void FlatMesh::getVertexBufferHandles(std::vector<VkBuffer>& handles) const
    {
        handles.emplace_back(vertexBuffer->get());
    }

    void FlatMesh::getVertexBufferOffsets(std::vector<size_t>& offsets) const
    {
        offsets.emplace_back(vertexBufferOffset);
    }

    size_t FlatMesh::getVertexBufferCount() const noexcept { return 1; }

    VkBuffer FlatMesh::getIndexBufferHandle() const noexcept { return VK_NULL_HANDLE; }

    size_t FlatMesh::getIndexBufferOffset() const noexcept { return 0; }

    VkIndexType FlatMesh::getIndexType() const noexcept { return VK_INDEX_TYPE_MAX_ENUM; }

    uint32_t FlatMesh::getVertexCount() const noexcept { return vertexCount; }

    uint32_t FlatMesh::getFirstVertex() const noexcept { return firstVertex; }

    uint32_t FlatMesh::getIndexCount() const noexcept { return 0; }

    uint32_t FlatMesh::getFirstIndex() const noexcept { return 0; }

    int32_t FlatMesh::getVertexOffset() const noexcept { return 0; }

    std::vector<VkAccessFlags> FlatMesh::getVertexBufferAccessFlags() const noexcept
    {
        std::vector flags = {accessFlags};
        return flags;
    }

    VkAccessFlags FlatMesh::getIndexBufferAccessFlags() const noexcept { return 0; }

    VulkanBuffer* FlatMesh::getVertexBuffer() const noexcept { return vertexBuffer.get(); }

    size_t FlatMesh::getVertexBufferOffset() const noexcept { return vertexBufferOffset; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void FlatMesh::setVertexBuffer(VulkanBufferPtr buffer) noexcept { vertexBuffer = std::move(buffer); }

    void FlatMesh::setVertexBufferOffset(const size_t offset) noexcept { vertexBufferOffset = offset; }

    void FlatMesh::setVertexCount(const uint32_t count) noexcept { vertexCount = count; }

    void FlatMesh::setFirstVertex(const uint32_t vertex) noexcept { firstVertex = vertex; }

    void FlatMesh::setVertexAccessFlags(const VkAccessFlags flags) noexcept { accessFlags = flags; }

    ////////////////////////////////////////////////////////////////
    // Update.
    ////////////////////////////////////////////////////////////////

    void FlatMesh::update(MeshDescriptionPtr desc) { getMeshManager().updateFlatMesh(*this, std::move(desc)); }

}  // namespace sol
