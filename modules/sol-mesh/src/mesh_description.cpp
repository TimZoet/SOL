#include "sol-mesh/mesh_description.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-error/sol_error.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MeshDescription::MeshDescription() = default;

    MeshDescription::MeshDescription(MeshManager& manager) : meshManager(&manager) {}

    MeshDescription::~MeshDescription() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MeshManager& MeshDescription::getMeshManager() noexcept { return *meshManager; }

    const MeshManager& MeshDescription::getMeshManager() const noexcept { return *meshManager; }

    bool MeshDescription::isIndexed() const noexcept { return indexBuffer.buffer != nullptr; }

    size_t MeshDescription::getVertexSize(const size_t buffer) const
    {
        if (buffer >= vertexBuffers.size()) throw SolError("Buffer index out of range.");
        return vertexBuffers[buffer].elementSize;
    }

    uint32_t MeshDescription::getVertexCount(const size_t buffer) const
    {
        if (buffer >= vertexBuffers.size()) throw SolError("Buffer index out of range.");
        return vertexBuffers[buffer].elementCount;
    }

    uint32_t MeshDescription::getVertexOffset(size_t buffer) const
    {
        if (buffer >= vertexBuffers.size()) throw SolError("Buffer index out of range.");
        return vertexBuffers[buffer].elementOffset;
    }

    VkBufferUsageFlags MeshDescription::getVertexFlags(const size_t buffer) const
    {
        if (buffer >= vertexBuffers.size()) throw SolError("Buffer index out of range.");
        return vertexBuffers[buffer].additionalFlags;
    }

    size_t MeshDescription::getVertexBufferCount() const noexcept { return vertexBuffers.size(); }

    const VulkanBuffer& MeshDescription::getVertexBuffer(const size_t buffer) const
    {
        if (buffer >= vertexBuffers.size()) throw SolError("Buffer index out of range.");
        return *vertexBuffers[buffer].buffer;
    }

    size_t MeshDescription::getIndexSize() const
    {
        if (!isIndexed()) throw SolError("No index buffer.");
        return indexBuffer.elementSize;
    }

    uint32_t MeshDescription::getIndexCount() const
    {
        if (!isIndexed()) throw SolError("No index buffer.");
        return indexBuffer.elementCount;
    }

    uint32_t MeshDescription::getIndexOffset() const
    {
        if (!isIndexed()) throw SolError("No index buffer.");
        return indexBuffer.elementOffset;
    }

    VkBufferUsageFlags MeshDescription::getIndexFlags() const
    {
        if (!isIndexed()) throw SolError("No index buffer.");
        return indexBuffer.additionalFlags;
    }

    const VulkanBuffer& MeshDescription::getIndexBuffer() const
    {
        if (!isIndexed()) throw SolError("No index buffer.");
        return *indexBuffer.buffer;
    }

    VkIndexType MeshDescription::getIndexType() const
    {
        switch (getIndexSize())
        {
        case 1: return VK_INDEX_TYPE_UINT8_EXT;
        case 2: return VK_INDEX_TYPE_UINT16;
        case 4: return VK_INDEX_TYPE_UINT32;
        default: throw SolError("Unsupported index size.");
        }
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MeshDescription::setVertexData(const size_t buffer, const size_t index, const void* data) const
    {
        if (buffer >= vertexBuffers.size()) throw SolError("Buffer index out of range.");
        if (index >= vertexBuffers[buffer].elementCount) throw SolError("Index out of range.");
        vertexBuffers[buffer].buffer->setData(
          data, vertexBuffers[buffer].elementSize, vertexBuffers[buffer].elementSize * index);
    }

    void MeshDescription::setVertexData(const size_t buffer,
                                        const size_t index,
                                        const size_t count,
                                        const void*  data) const
    {
        if (buffer >= vertexBuffers.size()) throw SolError("Buffer index out of range.");
        if (index + count > vertexBuffers[buffer].elementCount) throw SolError("Index out of range.");
        vertexBuffers[buffer].buffer->setData(
          data, vertexBuffers[buffer].elementSize * count, vertexBuffers[buffer].elementSize * index);
    }

    void MeshDescription::setIndexData(const size_t index, const void* data) const
    {
        if (!isIndexed()) throw SolError("Cannot set index data. No index buffer.");
        if (index >= indexBuffer.elementCount) throw SolError("Index out of range.");
        indexBuffer.buffer->setData(data, indexBuffer.elementSize, indexBuffer.elementSize * index);
    }

    void MeshDescription::setIndexData(const size_t index, const size_t count, const void* data) const
    {
        if (!isIndexed()) throw SolError("Cannot set index data. No index buffer.");
        if (index + count > indexBuffer.elementCount) throw SolError("Index out of range.");
        indexBuffer.buffer->setData(data, indexBuffer.elementSize * count, indexBuffer.elementSize * index);
    }

    ////////////////////////////////////////////////////////////////
    // Buffers.
    ////////////////////////////////////////////////////////////////

    size_t MeshDescription::addVertexBuffer(const size_t             vertexSize,
                                            const uint32_t           vertexCount,
                                            const VkBufferUsageFlags additionalFlags)
    {
        return addVertexBuffer(vertexSize, vertexCount, 0, additionalFlags);
    }

    size_t MeshDescription::addVertexBuffer(size_t                   vertexSize,
                                            uint32_t                 vertexCount,
                                            uint32_t                 vertexOffset,
                                            const VkBufferUsageFlags additionalFlags)
    {
        if (vertexSize == 0) throw SolError("Cannot add vertex buffer with vertex size 0.");
        if (vertexCount == 0) throw SolError("Cannot add vertex buffer with vertex count 0.");

        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device          = meshManager->getMemoryManager().getDevice();
        bufferSettings.bufferUsage     = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferSettings.sharingMode     = VK_SHARING_MODE_EXCLUSIVE;
        bufferSettings.allocator       = meshManager->getMemoryManager().getAllocator();
        bufferSettings.vma.memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
        bufferSettings.vma.flags       = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        bufferSettings.size            = vertexSize * vertexCount;

        vertexBuffers.emplace_back(
          VulkanBuffer::create(bufferSettings), vertexSize, vertexCount, vertexOffset, additionalFlags);

        return vertexBuffers.size() - 1;
    }

    void MeshDescription::addIndexBuffer(const size_t             indexSize,
                                         const uint32_t           indexCount,
                                         const VkBufferUsageFlags additionalFlags)
    {
        addIndexBuffer(indexSize, indexCount, 0, additionalFlags);
    }

    void MeshDescription::addIndexBuffer(const size_t             indexSize,
                                         const uint32_t           indexCount,
                                         const uint32_t           indexOffset,
                                         const VkBufferUsageFlags additionalFlags)
    {
        if (indexBuffer.buffer) throw SolError("Cannot add index buffer. MeshDescription already has one.");
        if (indexSize == 0) throw SolError("Cannot add index buffer with index size 0.");
        if (indexCount == 0) throw SolError("Cannot add index buffer with index count 0.");

        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device          = meshManager->getMemoryManager().getDevice();
        bufferSettings.bufferUsage     = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferSettings.sharingMode     = VK_SHARING_MODE_EXCLUSIVE;
        bufferSettings.allocator       = meshManager->getMemoryManager().getAllocator();
        bufferSettings.vma.memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
        bufferSettings.vma.flags       = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        bufferSettings.size            = indexSize * indexCount;

        indexBuffer.buffer          = VulkanBuffer::create(bufferSettings);
        indexBuffer.elementSize     = indexSize;
        indexBuffer.elementCount    = indexCount;
        indexBuffer.elementOffset   = indexOffset;
        indexBuffer.additionalFlags = additionalFlags;
    }

}  // namespace sol
