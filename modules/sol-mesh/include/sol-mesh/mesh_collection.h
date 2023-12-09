#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/i_mesh.h"

namespace sol
{
    class IndexedMesh final : public IMesh
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IndexedMesh();

        /**
         * \brief Construct an IndexedMesh.
         * \param manager MeshManager.
         * \param id Identifier.
         */
        IndexedMesh(MeshManager& manager, uuids::uuid id);

        IndexedMesh(const IndexedMesh&) = delete;

        IndexedMesh(IndexedMesh&&) = delete;

        ~IndexedMesh() noexcept override;

        IndexedMesh& operator=(const IndexedMesh&) = delete;

        IndexedMesh& operator=(IndexedMesh&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] MeshType getType() const noexcept override;

        [[nodiscard]] bool isValid() const noexcept override;

        [[nodiscard]] bool isIndexed() const noexcept override;

        void getVertexBufferHandles(std::vector<VkBuffer>& handles) const override;

        void getVertexBufferOffsets(std::vector<size_t>& offsets) const override;

        [[nodiscard]] size_t getVertexBufferCount() const noexcept override;

        [[nodiscard]] VkBuffer getIndexBufferHandle() const noexcept override;

        [[nodiscard]] size_t getIndexBufferOffset() const noexcept override;

        [[nodiscard]] VkIndexType getIndexType() const noexcept override;

        [[nodiscard]] uint32_t getVertexCount() const noexcept override;

        [[nodiscard]] uint32_t getFirstVertex() const noexcept override;

        [[nodiscard]] uint32_t getIndexCount() const noexcept override;

        [[nodiscard]] uint32_t getFirstIndex() const noexcept override;

        [[nodiscard]] int32_t getVertexOffset() const noexcept override;

        [[nodiscard]] std::vector<VkAccessFlags> getVertexBufferAccessFlags() const noexcept override;

        [[nodiscard]] VkAccessFlags getIndexBufferAccessFlags() const noexcept override;

        [[nodiscard]] VulkanBuffer* getVertexBuffer() const noexcept;

        [[nodiscard]] size_t getVertexBufferOffset() const noexcept;

        [[nodiscard]] VulkanBuffer* getIndexBuffer() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setVertexBuffer(VulkanBufferPtr buffer) noexcept;

        void setVertexBufferOffset(size_t offset) noexcept;

        void setVertexCount(uint32_t count) noexcept;

        void setIndexBuffer(VulkanBufferPtr buffer) noexcept;

        void setIndexBufferOffset(size_t offset) noexcept;

        void setIndexType(VkIndexType type) noexcept;

        void setIndexCount(uint32_t count) noexcept;

        void setFirstIndex(uint32_t index) noexcept;

        void setVertexOffset(int32_t offset) noexcept;

        void setVertexAccessFlags(VkAccessFlags flags) noexcept;

        void setIndexAccessFlags(VkAccessFlags flags) noexcept;

        ////////////////////////////////////////////////////////////////
        // Update.
        ////////////////////////////////////////////////////////////////

        void update(MeshDescriptionPtr desc) override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanBufferPtr vertexBuffer;

        VulkanBufferPtr indexBuffer;

        size_t vertexBufferOffset = 0;

        size_t indexBufferOffset = 0;

        VkIndexType indexType = VK_INDEX_TYPE_MAX_ENUM;

        uint32_t indexCount = 0;

        uint32_t firstIndex = 0;

        int32_t vertexOffset = 0;

        uint32_t vertexCount = 0;

        VkAccessFlags vertexAccessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

        VkAccessFlags indexAccessFlags = VK_ACCESS_INDEX_READ_BIT;
    };
}  // namespace sol
