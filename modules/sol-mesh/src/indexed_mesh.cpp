#include "sol-mesh/indexed_mesh.h"

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

    IndexedMesh::IndexedMesh() = default;

    IndexedMesh::IndexedMesh(MeshManager& manager, const uuids::uuid id) : IMesh(manager, id) {}

    IndexedMesh::~IndexedMesh() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMesh::MeshType IndexedMesh::getType() const noexcept { return MeshType::Indexed; }

    bool IndexedMesh::isValid() const noexcept { return vertexBuffer && indexBuffer && indexCount > 0; }

    bool IndexedMesh::isIndexed() const noexcept { return true; }

    void IndexedMesh::getVertexBufferHandles(std::vector<VkBuffer>& handles) const
    {
        handles.emplace_back(vertexBuffer->get());
    }

    void IndexedMesh::getVertexBufferOffsets(std::vector<size_t>& offsets) const
    {
        offsets.emplace_back(vertexBufferOffset);
    }

    size_t IndexedMesh::getVertexBufferCount() const noexcept { return 1; }

    VkBuffer IndexedMesh::getIndexBufferHandle() const noexcept { return indexBuffer->get(); }

    size_t IndexedMesh::getIndexBufferOffset() const noexcept { return indexBufferOffset; }

    VkIndexType IndexedMesh::getIndexType() const noexcept { return indexType; }

    uint32_t IndexedMesh::getVertexCount() const noexcept { return vertexCount; }

    uint32_t IndexedMesh::getFirstVertex() const noexcept { return 0; }

    uint32_t IndexedMesh::getIndexCount() const noexcept { return indexCount; }

    uint32_t IndexedMesh::getFirstIndex() const noexcept { return firstIndex; }

    int32_t IndexedMesh::getVertexOffset() const noexcept { return vertexOffset; }

    std::vector<VkAccessFlags> IndexedMesh::getVertexBufferAccessFlags() const noexcept
    {
        std::vector flags = {vertexAccessFlags};
        return flags;
    }

    VkAccessFlags IndexedMesh::getIndexBufferAccessFlags() const noexcept { return indexAccessFlags; }

    VulkanBuffer* IndexedMesh::getVertexBuffer() const noexcept { return vertexBuffer.get(); }

    size_t IndexedMesh::getVertexBufferOffset() const noexcept { return vertexBufferOffset; }

    VulkanBuffer* IndexedMesh::getIndexBuffer() const noexcept { return indexBuffer.get(); }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void IndexedMesh::setVertexBuffer(VulkanBufferPtr buffer) noexcept { vertexBuffer = std::move(buffer); }

    void IndexedMesh::setVertexBufferOffset(const size_t offset) noexcept { vertexBufferOffset = offset; }

    void IndexedMesh::setVertexCount(const uint32_t count) noexcept { vertexCount = count; }

    void IndexedMesh::setIndexBuffer(VulkanBufferPtr buffer) noexcept { indexBuffer = std::move(buffer); }

    void IndexedMesh::setIndexBufferOffset(const size_t offset) noexcept { indexBufferOffset = offset; }

    void IndexedMesh::setIndexType(const VkIndexType type) noexcept { indexType = type; }

    void IndexedMesh::setIndexCount(const uint32_t count) noexcept { indexCount = count; }

    void IndexedMesh::setFirstIndex(const uint32_t index) noexcept { firstIndex = index; }

    void IndexedMesh::setVertexOffset(const int32_t offset) noexcept { vertexOffset = offset; }

    void IndexedMesh::setVertexAccessFlags(const VkAccessFlags flags) noexcept { vertexAccessFlags = flags; }

    void IndexedMesh::setIndexAccessFlags(const VkAccessFlags flags) noexcept { indexAccessFlags = flags; }

    ////////////////////////////////////////////////////////////////
    // Update.
    ////////////////////////////////////////////////////////////////

    void IndexedMesh::update(MeshDescriptionPtr desc) { getMeshManager().updateIndexedMesh(*this, std::move(desc)); }
}  // namespace sol
