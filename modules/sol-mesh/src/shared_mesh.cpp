#include "sol-mesh/shared_mesh.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    SharedMesh::SharedMesh() = default;

    SharedMesh::SharedMesh(MeshManager& manager, const uuids::uuid id) : IMesh(manager, id) {}

    SharedMesh::~SharedMesh() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IMesh::MeshType SharedMesh::getType() const noexcept { return MeshType::Shared; }

    bool SharedMesh::isValid() const noexcept
    {
        // At least one vertex buffer is needed.
        if (vertexBuffers.empty()) return false;

        // And all vertex buffers need to be valid.
        for (const auto* mesh : vertexBuffers)
            if (!mesh->isValid()) return false;

        // If indexed, verify index buffer and count.
        if (isIndexed())
        {
            if (!indexBuffer->isValid()) return false;
            if (indexCount && *indexCount == 0) return false;
        }
        // Otherwise verify vertex count.
        else
        {
            if (vertexCount && *vertexCount == 0) return false;
        }

        // If there are custom offsets, verify there is an offset for each vertex buffer.
        if (!vertexBufferOffsets.empty() && vertexBufferOffsets.size() != getVertexBufferCount()) return false;

        return true;
    }

    bool SharedMesh::isIndexed() const noexcept { return indexBuffer ? indexBuffer->isIndexed() : false; }

    void SharedMesh::getVertexBufferHandles(std::vector<VkBuffer>& handles) const
    {
        for (const auto* mesh : vertexBuffers) mesh->getVertexBufferHandles(handles);
    }

    void SharedMesh::getVertexBufferOffsets(std::vector<size_t>& offsets) const
    {
        if (!vertexBufferOffsets.empty())
            for (const auto o : vertexBufferOffsets) offsets.emplace_back(o);
        else
            for (const auto* mesh : vertexBuffers) mesh->getVertexBufferOffsets(offsets);
    }

    size_t SharedMesh::getVertexBufferCount() const noexcept
    {
        size_t count = 0;
        for (const auto* mesh : vertexBuffers) count += mesh->getVertexBufferCount();
        return count;
    }

    VkBuffer SharedMesh::getIndexBufferHandle() const noexcept
    {
        return indexBuffer ? indexBuffer->getIndexBufferHandle() : VK_NULL_HANDLE;
    }

    size_t SharedMesh::getIndexBufferOffset() const noexcept
    {
        if (indexBufferOffset) return *indexBufferOffset;
        if (indexBuffer) return indexBuffer->getIndexBufferOffset();
        return 0;
    }

    VkIndexType SharedMesh::getIndexType() const noexcept
    {
        return indexBuffer ? indexBuffer->getIndexType() : VK_INDEX_TYPE_MAX_ENUM;
    }

    uint32_t SharedMesh::getVertexCount() const noexcept
    {
        if (vertexCount) return *vertexCount;
        if (!vertexBuffers.empty()) return vertexBuffers.front()->getVertexCount();
        return 0;
    }

    uint32_t SharedMesh::getFirstVertex() const noexcept
    {
        if (firstVertex) return *firstVertex;
        if (!vertexBuffers.empty()) return vertexBuffers.front()->getFirstVertex();
        return 0;
    }

    uint32_t SharedMesh::getIndexCount() const noexcept
    {
        if (indexCount) return *indexCount;
        if (indexBuffer) return indexBuffer->getIndexCount();
        return 0;
    }

    uint32_t SharedMesh::getFirstIndex() const noexcept
    {
        if (firstIndex) return *firstIndex;
        if (indexBuffer) return indexBuffer->getFirstIndex();
        return 0;
    }

    int32_t SharedMesh::getVertexOffset() const noexcept
    {
        if (vertexOffset) return *vertexOffset;
        if (indexBuffer) return indexBuffer->getVertexOffset();
        return 0;
    }

    std::vector<VkAccessFlags> SharedMesh::getVertexBufferAccessFlags() const noexcept { return {}; }

    VkAccessFlags SharedMesh::getIndexBufferAccessFlags() const noexcept { return 0; }

    IMesh& SharedMesh::getVertexBuffer(const size_t index) const
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot get vertex buffer of SharedMesh: index out of bounds.");
        return *vertexBuffers[index];
    }

    IMesh* SharedMesh::getIndexBuffer() const noexcept { return indexBuffer; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void SharedMesh::addVertexBuffer(IMesh& mesh) { vertexBuffers.emplace_back(&mesh); }

    void SharedMesh::addVertexBufferOffset(const uint32_t offset) { vertexBufferOffsets.emplace_back(offset); }

    void SharedMesh::setVertexBuffer(IMesh& mesh, const size_t index)
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot set vertex buffer of SharedMesh: index out of bounds.");
        vertexBuffers[index] = &mesh;
    }

    void SharedMesh::setVertexBufferOffset(const uint32_t offset, const size_t index)
    {
        if (index >= vertexBufferOffsets.size())
            throw SolError("Cannot set vertex buffer offset of SharedMesh: index out of bounds.");
        vertexBufferOffsets[index] = offset;
    }

    void SharedMesh::removeVertexBuffer(const size_t index)
    {
        if (index >= vertexBuffers.size())
            throw SolError("Cannot remove vertex buffer from SharedMesh: index out of bounds.");
        vertexBuffers.erase(vertexBuffers.begin() + static_cast<std::make_signed_t<size_t>>(index));
    }

    void SharedMesh::setIndexBuffer(IMesh* mesh) noexcept { indexBuffer = mesh; }

    void SharedMesh::setIndexBufferOffset(const std::optional<size_t> offset) noexcept { indexBufferOffset = offset; }

    void SharedMesh::setVertexCount(const std::optional<uint32_t> count) noexcept { vertexCount = count; }

    void SharedMesh::setFirstVertex(const std::optional<uint32_t> vertex) noexcept { firstVertex = vertex; }

    void SharedMesh::setIndexCount(const std::optional<uint32_t> count) noexcept { indexCount = count; }

    void SharedMesh::setFirstIndex(const std::optional<uint32_t> index) noexcept { firstIndex = index; }

    void SharedMesh::setVertexOffset(const std::optional<int32_t> offset) noexcept { vertexOffset = offset; }
}  // namespace sol
