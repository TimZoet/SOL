#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh.h"

namespace sol
{
    class MeshCollection
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MeshCollection() = delete;

        explicit MeshCollection(GeometryBufferAllocator& alloc);

        MeshCollection(const MeshCollection&) = delete;

        MeshCollection(MeshCollection&&) = delete;

        ~MeshCollection() noexcept;

        MeshCollection& operator=(const MeshCollection&) = delete;

        MeshCollection& operator=(MeshCollection&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the geometry buffer allocator this mesh collection is using.
         * \return GeometryBufferAllocator.
         */
        [[nodiscard]] GeometryBufferAllocator& getAllocator() noexcept;

        /**
         * \brief Get the geometry buffer allocator this mesh collection is using.
         * \return GeometryBufferAllocator.
         */
        [[nodiscard]] const GeometryBufferAllocator& getAllocator() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Meshes.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Allocate a new vertex buffer. Reroutes call to the internal allocator.
         * \param count Number of vertices.
         * \param size Size of each vertex in bytes. Ignored if strategy == Global.
         * \return Vertex buffer.
         */
        [[nodiscard]] VertexBufferPtr allocateVertexBuffer(size_t count, size_t size = 0) const;

        /**
         * \brief Allocate a new index buffer. Reroutes call to the internal allocator.
         * \param count Number of indices.
         * \param size Size of each index in bytes. Ignored if strategy == Global.
         * \return Index buffer.
         */
        [[nodiscard]] IndexBufferPtr allocateIndexBuffer(size_t count, size_t size = 0) const;

        /**
         * \brief Create a new mesh.
         * \param vertexBuffer Vertex buffer.
         * \param indexBuffer Optional index buffer.
         * \param vertexBuffers List of additional vertex buffers.
         * \return Mesh.
         */
        template<std::same_as<VertexBufferPtr>... Ts>
        [[nodiscard]] Mesh& createMesh(VertexBufferPtr vertexBuffer, IndexBufferPtr indexBuffer, Ts&&... vertexBuffers)
        {
            auto& mesh = createMeshImpl();
            mesh.vertexBuffers.reserve(sizeof...(Ts) + 1);
            (mesh.vertexBuffers.push_back(std::move(vertexBuffer)),
             ...,
             mesh.vertexBuffers.push_back(std::move(vertexBuffers)));
            mesh.indexBuffer = std::move(indexBuffer);
            return mesh;
        }

        void destroyMesh(const Mesh& mesh);

    private:
        [[nodiscard]] Mesh& createMeshImpl();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        GeometryBufferAllocator* allocator = nullptr;

        std::unordered_map<uuids::uuid, MeshPtr> meshes;
    };
}  // namespace sol
