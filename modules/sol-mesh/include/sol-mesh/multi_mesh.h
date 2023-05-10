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
    class MultiMesh final : public IMesh
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MultiMesh();

        /**
         * \brief Construct a MultiMesh.
         * \param manager MeshManager.
         * \param id Identifier.
         */
        MultiMesh(MeshManager& manager, uuids::uuid id);

        MultiMesh(const MultiMesh&) = delete;

        MultiMesh(MultiMesh&&) = delete;

        ~MultiMesh() noexcept override;

        MultiMesh& operator=(const MultiMesh&) = delete;

        MultiMesh& operator=(MultiMesh&&) = delete;

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

        [[nodiscard]] VulkanBuffer& getVertexBuffer(size_t index) const;

        [[nodiscard]] VulkanBuffer* getIndexBuffer() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void addVertexBuffer(VulkanBufferPtr buffer);

        void setVertexBuffer(VulkanBufferPtr buffer, size_t index);

        void setVertexBufferOffset(size_t offset, size_t index);

        void removeVertexBuffer(size_t index);

        void setIndexBuffer(VulkanBufferPtr buffer) noexcept;

        void setIndexBufferOffset(size_t offset) noexcept;

        void setIndexType(VkIndexType type) noexcept;

        void setVertexCount(uint32_t count) noexcept;

        void setFirstVertex(uint32_t vertex) noexcept;

        void setIndexCount(uint32_t count) noexcept;

        void setFirstIndex(uint32_t index) noexcept;

        void setVertexOffset(int32_t offset) noexcept;

        void setVertexAccessFlags(VkAccessFlags flags, size_t index);

        void setIndexAccessFlags(VkAccessFlags flags) noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<VulkanBufferPtr> vertexBuffers;

        VulkanBufferPtr indexBuffer;

        std::vector<size_t> vertexBufferOffsets;

        size_t indexBufferOffset = 0;

        VkIndexType indexType = VK_INDEX_TYPE_MAX_ENUM;

        uint32_t vertexCount = 0;

        uint32_t firstVertex = 0;

        uint32_t indexCount = 0;

        uint32_t firstIndex = 0;

        int32_t vertexOffset = 0;

        std::vector<VkAccessFlags> vertexAccessFlags;

        VkAccessFlags indexAccessFlags = VK_ACCESS_INDEX_READ_BIT;
    };
}  // namespace sol
