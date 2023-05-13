#include "sol-mesh/multi_mesh.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MultiMesh::MultiMesh() = default;

    MultiMesh::MultiMesh(MeshManager& manager, const uuids::uuid id) : IMesh(manager, id) {}

    MultiMesh::~MultiMesh() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMesh::MeshType MultiMesh::getType() const noexcept { return MeshType::Multi; }

    bool MultiMesh::isValid() const noexcept
    {
        if (!indexBuffer) return !vertexBuffers.empty() && vertexCount > 0;
        return !vertexBuffers.empty() && indexBuffer && indexCount > 0;
    }

    bool MultiMesh::isIndexed() const noexcept { return indexBuffer != nullptr; }

    void MultiMesh::getVertexBufferHandles(std::vector<VkBuffer>& handles) const
    {
        for (const auto& buffer : vertexBuffers) handles.emplace_back(buffer->get());
    }

    void MultiMesh::getVertexBufferOffsets(std::vector<size_t>& offsets) const
    {
        for (auto o : vertexBufferOffsets) offsets.emplace_back(o);
    }

    size_t MultiMesh::getVertexBufferCount() const noexcept { return vertexBuffers.size(); }

    VkBuffer MultiMesh::getIndexBufferHandle() const noexcept
    {
        return indexBuffer ? indexBuffer->get() : VK_NULL_HANDLE;
    }

    size_t MultiMesh::getIndexBufferOffset() const noexcept { return indexBufferOffset; }

    VkIndexType MultiMesh::getIndexType() const noexcept { return indexType; }

    uint32_t MultiMesh::getVertexCount() const noexcept { return vertexCount; }

    uint32_t MultiMesh::getFirstVertex() const noexcept { return firstVertex; }

    uint32_t MultiMesh::getIndexCount() const noexcept { return indexCount; }

    uint32_t MultiMesh::getFirstIndex() const noexcept { return firstIndex; }

    int32_t MultiMesh::getVertexOffset() const noexcept { return vertexOffset; }

    std::vector<VkAccessFlags> MultiMesh::getVertexBufferAccessFlags() const noexcept { return vertexAccessFlags; }

    VkAccessFlags MultiMesh::getIndexBufferAccessFlags() const noexcept { return indexAccessFlags; }

    VulkanBuffer& MultiMesh::getVertexBuffer(const size_t index) const
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot get vertex buffer from MultiMesh: index out of bounds.");
        return *vertexBuffers[index];
    }

    VulkanBuffer* MultiMesh::getIndexBuffer() const noexcept { return indexBuffer.get(); }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MultiMesh::addVertexBuffer(VulkanBufferPtr buffer)
    {
        if (!buffer) throw SolError("Cannot add vertex buffer to MultiMesh: buffer is null.");

        vertexBuffers.emplace_back(std::move(buffer));
        vertexBufferOffsets.emplace_back(0);
        vertexAccessFlags.emplace_back(VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);
    }

    void MultiMesh::setVertexBuffer(VulkanBufferPtr buffer, const size_t index)
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot set vertex buffer of MultiMesh: index out of bounds.");
        if (!buffer) throw SolError("Cannot set vertex buffer of MultiMesh: buffer is null.");

        vertexBuffers[index] = std::move(buffer);
    }

    void MultiMesh::setVertexBufferOffset(const size_t offset, const size_t index)
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot set vertex buffer offset of MultiMesh: index out of bounds.");

        vertexBufferOffsets[index] = offset;
    }

    void MultiMesh::removeVertexBuffer(const size_t index)
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot remove vertex buffer from MultiMesh: index out of bounds.");

        const auto offset = static_cast<std::make_signed_t<size_t>>(index);
        vertexBuffers.erase(vertexBuffers.begin() + offset);
        vertexBufferOffsets.erase(vertexBufferOffsets.begin() + offset);
        vertexAccessFlags.erase(vertexAccessFlags.begin() + offset);
    }

    void MultiMesh::setIndexBuffer(VulkanBufferPtr buffer) noexcept { indexBuffer = std::move(buffer); }

    void MultiMesh::setIndexBufferOffset(const size_t offset) noexcept { indexBufferOffset = offset; }

    void MultiMesh::setIndexType(const VkIndexType type) noexcept { indexType = type; }

    void MultiMesh::setVertexCount(const uint32_t count) noexcept { vertexCount = count; }

    void MultiMesh::setFirstVertex(const uint32_t vertex) noexcept { firstVertex = vertex; }

    void MultiMesh::setIndexCount(const uint32_t count) noexcept { indexCount = count; }

    void MultiMesh::setFirstIndex(const uint32_t index) noexcept { firstIndex = index; }

    void MultiMesh::setVertexOffset(const int32_t offset) noexcept { vertexOffset = offset; }

    void MultiMesh::setVertexAccessFlags(const VkAccessFlags flags, const size_t index)
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot set vertex access flags of MultiMesh: index out of bounds.");
        vertexAccessFlags[index] = flags;
    }

    void MultiMesh::setIndexAccessFlags(const VkAccessFlags flags) noexcept { indexAccessFlags = flags; }
}  // namespace sol
