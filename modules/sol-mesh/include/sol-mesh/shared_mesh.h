#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/i_mesh.h"

namespace sol
{
    class SharedMesh final : public IMesh
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SharedMesh();

        /**
         * \brief Construct a SharedMesh.
         * \param manager MeshManager.
         * \param id Identifier.
         */
        SharedMesh(MeshManager& manager, uuids::uuid id);

        SharedMesh(const SharedMesh&) = delete;

        SharedMesh(SharedMesh&&) = delete;

        ~SharedMesh() noexcept override;

        SharedMesh& operator=(const SharedMesh&) = delete;

        SharedMesh& operator=(SharedMesh&&) = delete;

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

        [[nodiscard]] IMesh& getVertexBuffer(size_t index) const;

        [[nodiscard]] IMesh* getIndexBuffer() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void addVertexBuffer(IMesh& mesh);

        void addVertexBufferOffset(uint32_t offset);

        void setVertexBuffer(IMesh& mesh, size_t index);

        void setVertexBufferOffset(uint32_t offset, size_t index);

        void removeVertexBuffer(size_t index);

        void setIndexBuffer(IMesh* mesh) noexcept;

        void setIndexBufferOffset(std::optional<size_t> offset) noexcept;

        void setVertexCount(std::optional<uint32_t> count) noexcept;

        void setFirstVertex(std::optional<uint32_t> vertex) noexcept;

        void setIndexCount(std::optional<uint32_t> count) noexcept;

        void setFirstIndex(std::optional<uint32_t> index) noexcept;

        void setVertexOffset(std::optional<int32_t> offset) noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<IMesh*> vertexBuffers;

        IMesh* indexBuffer = nullptr;

        std::vector<size_t> vertexBufferOffsets;

        std::optional<size_t> indexBufferOffset;

        std::optional<uint32_t> vertexCount;

        std::optional<uint32_t> firstVertex;

        std::optional<uint32_t> indexCount;

        std::optional<uint32_t> firstIndex;

        std::optional<int32_t> vertexOffset;
    };
}  // namespace sol
