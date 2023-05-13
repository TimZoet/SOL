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
    class FlatMesh final : public IMesh
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        FlatMesh();

        /**
         * \brief Construct a FlatMesh.
         * \param manager MeshManager.
         * \param id Identifier.
         */
        FlatMesh(MeshManager& manager, uuids::uuid id);

        /**
         * \brief Construct a FlatMesh.
         * \param manager MeshManager.
         * \param id Identifier.
         * \param vtxBuffer Vertex buffer.
         * \param vtxCount Vertex count.
         */
        FlatMesh(MeshManager& manager, uuids::uuid id, VulkanBufferPtr vtxBuffer, uint32_t vtxCount);

        FlatMesh(const FlatMesh&) = delete;

        FlatMesh(FlatMesh&&) = delete;

        ~FlatMesh() noexcept override;

        FlatMesh& operator=(const FlatMesh&) = delete;

        FlatMesh& operator=(FlatMesh&&) = delete;

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

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setVertexBuffer(VulkanBufferPtr buffer) noexcept;

        void setVertexBufferOffset(size_t offset) noexcept;

        void setVertexCount(uint32_t count) noexcept;

        void setFirstVertex(uint32_t vertex) noexcept;

        void setVertexAccessFlags(VkAccessFlags flags) noexcept;

        ////////////////////////////////////////////////////////////////
        // Update.
        ////////////////////////////////////////////////////////////////

        void update(MeshDescriptionPtr desc) override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanBufferPtr vertexBuffer;

        size_t vertexBufferOffset = 0;

        uint32_t vertexCount = 0;

        uint32_t firstVertex = 0;

        VkAccessFlags accessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    };
}  // namespace sol