#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class MeshDescription
    {
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        struct Buffer
        {
            VulkanBufferPtr buffer;

            size_t elementSize = 0;

            uint32_t elementCount = 0;

            /**
             * \brief Offset to the first element in the target buffer to copy to.
             */
            uint32_t elementOffset = 0;

            /**
             * \brief Additional flags for the final buffer.
             */
            VkBufferUsageFlags additionalFlags = 0;
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MeshDescription();

        explicit MeshDescription(MeshManager& manager);

        MeshDescription(const MeshDescription&) = delete;

        MeshDescription(MeshDescription&&) = delete;

        ~MeshDescription() noexcept;

        MeshDescription& operator=(const MeshDescription&) = delete;

        MeshDescription& operator=(MeshDescription&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the MeshManager.
         * \return MeshManager.
         */
        [[nodiscard]] MeshManager& getMeshManager() noexcept;

        /**
         * \brief Get the MeshManager.
         * \return MeshManager.
         */
        [[nodiscard]] const MeshManager& getMeshManager() const noexcept;

        /**
         * \brief Returns whether this description has an index buffer.
         * \return True if there is an index buffer, false otherwise.
         */
        [[nodiscard]] bool isIndexed() const noexcept;

        /**
         * \brief Get the size of a single vertex in bytes.
         * \return Size.
         */
        [[nodiscard]] size_t getVertexSize(size_t buffer) const;

        /**
         * \brief Get the number of vertices.
         * \return Number of vertices.
         */
        [[nodiscard]] uint32_t getVertexCount(size_t buffer) const;

        /**
         * \brief Get the target vertex offset.
         * \return Offset.
         */
        [[nodiscard]] uint32_t getVertexOffset(size_t buffer) const;

        /**
         * \brief Get the additional vertex buffer usage flags.
         * \return Flags.
         */
        [[nodiscard]] VkBufferUsageFlags getVertexFlags(size_t buffer) const;

        /**
         * \brief Get the number of vertex buffers.
         * \return Number of vertex buffers.
         */
        [[nodiscard]] size_t getVertexBufferCount() const noexcept;

        /**
         * \brief Get the vertex staging buffer at the given index.
         * \param buffer Buffer index.
         * \return Vertex buffer.
         */
        [[nodiscard]] const VulkanBuffer& getVertexBuffer(size_t buffer) const;

        /**
         * \brief Get the size of a single index in bytes.
         * \return Size.
         */
        [[nodiscard]] size_t getIndexSize() const;

        /**
         * \brief Get the number of indices.
         * \return Number of indices.
         */
        [[nodiscard]] uint32_t getIndexCount() const;

        /**
         * \brief Get the target index offset.
         * \return Offset.
         */
        [[nodiscard]] uint32_t getIndexOffset() const;

        /**
         * \brief Get the additional index buffer usage flags.
         * \return Flags.
         */
        [[nodiscard]] VkBufferUsageFlags getIndexFlags() const;

        /**
         * \brief Get the index staging buffer.
         * \return Index buffer.
         */
        [[nodiscard]] const VulkanBuffer& getIndexBuffer() const;

        /**
         * \brief Get the index type. This value is automatically derived from the index size.
         * \return VkIndexType.
         */
        [[nodiscard]] VkIndexType getIndexType() const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set a single vertex element.
         * \param buffer Buffer index.
         * \param index Element index.
         * \param data Vertex data. Should point to at least getVertexSize(buffer) bytes.
         */
        void setVertexData(size_t buffer, size_t index, const void* data) const;

        /**
         * \brief Set multiple vertex elements.
         * \param buffer Buffer index.
         * \param index First element index.
         * \param count Number of vertex elements to set.
         * \param data Vertex data. Should point to at least count * getVertexSize(buffer) bytes.
         */
        void setVertexData(size_t buffer, size_t index, size_t count, const void* data) const;

        /**
         * \brief Set a single index element.
         * \param index Element index.
         * \param data Index data. Should point to at least getIndexSize() bytes.
         */
        void setIndexData(size_t index, const void* data) const;

        /**
         * \brief Set multiple index elements.
         * \param index First element index.
         * \param count Number of index elements to set.
         * \param data Index data. Should point to at least count * getIndexSize() bytes.
         */
        void setIndexData(size_t index, size_t count, const void* data) const;

        ////////////////////////////////////////////////////////////////
        // Buffers.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add a vertex buffer.
         * \param vertexSize Size of a single vertex in bytes.
         * \param vertexCount Number of vertices.
         * \param additionalFlags Additional buffer usage flags to assign to vertex buffer.
         * \return Index of the new buffer.
         */
        size_t addVertexBuffer(size_t vertexSize, uint32_t vertexCount, VkBufferUsageFlags additionalFlags = 0);

        /**
         * \brief Add a vertex buffer.
         * \param vertexSize Size of a single vertex in bytes.
         * \param vertexCount Number of vertices.
         * \param vertexOffset Offset to the first vertex in the target buffer to copy to.
         * \param additionalFlags Additional buffer usage flags to assign to vertex buffer.
         * \return Index of the new buffer.
         */
        size_t addVertexBuffer(size_t             vertexSize,
                               uint32_t           vertexCount,
                               uint32_t           vertexOffset,
                               VkBufferUsageFlags additionalFlags = 0);

        /**
         * \brief Add an index buffer.
         * \param indexSize Size of a single index in bytes.
         * \param additionalFlags Additional buffer usage flags to assign to vertex buffer.
         * \param indexCount Number of indices.
         */
        void addIndexBuffer(size_t indexSize, uint32_t indexCount, VkBufferUsageFlags additionalFlags = 0);

        /**
         * \brief Add an index buffer.
         * \param indexSize Size of a single index in bytes.
         * \param indexCount Number of indices.
         * \param indexOffset Offset to the first index in the target buffer to copy to.
         * \param additionalFlags Additional buffer usage flags to assign to vertex buffer.
         */
        void addIndexBuffer(size_t             indexSize,
                            uint32_t           indexCount,
                            uint32_t           indexOffset,
                            VkBufferUsageFlags additionalFlags = 0);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief MeshManager.
         */
        MeshManager* meshManager = nullptr;

        /**
         * \brief Vertex staging buffers.
         */
        std::vector<Buffer> vertexBuffers;

        /**
         * \brief Index staging buffer.
         */
        Buffer indexBuffer;
    };
}  // namespace sol