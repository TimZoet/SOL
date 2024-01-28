#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class Mesh
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Mesh();

        explicit Mesh(VertexBufferPtr vb0);

        Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1);

        Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2);

        Mesh(VertexBufferPtr vb0, IndexBufferPtr ib);

        Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1, IndexBufferPtr ib);

        Mesh(VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2, IndexBufferPtr ib);

        explicit Mesh(uuids::uuid id);

        Mesh(uuids::uuid id, VertexBufferPtr vb0);

        Mesh(uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1);

        Mesh(uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2);

        Mesh(uuids::uuid id, VertexBufferPtr vb0, IndexBufferPtr ib);

        Mesh(uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1, IndexBufferPtr ib);

        Mesh(uuids::uuid id, VertexBufferPtr vb0, VertexBufferPtr vb1, VertexBufferPtr vb2, IndexBufferPtr ib);

        Mesh(const Mesh&) = delete;

        Mesh(Mesh&&) = delete;

        ~Mesh() noexcept;

        Mesh& operator=(const Mesh&) = delete;

        Mesh& operator=(Mesh&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the UUID.
         * \return UUID.
         */
        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        /**
         * \brief Get the number of vertex buffers this mesh has.
         * \return Number of buffers.
         */
        [[nodiscard]] size_t getVertexBufferCount() const noexcept;

        /**
         * \brief Get whether this mesh has an index buffer.
         * \return True or false.
         */
        [[nodiscard]] bool hasIndexBuffer() const noexcept;

        /**
         * \brief Get the vertex buffers.
         * \return List of vertex buffers.
         */
        [[nodiscard]] std::vector<VertexBufferPtr>& getVertexBuffers() noexcept;

        /**
         * \brief Get the vertex buffers.
         * \return List of vertex buffers.
         */
        [[nodiscard]] const std::vector<VertexBufferPtr>& getVertexBuffers() const noexcept;

        /**
         * \brief Get the index buffer, if any.
         * \return Index buffer or null.
         */
        [[nodiscard]] IndexBufferPtr& getIndexBuffer() noexcept;

        /**
         * \brief Get the index buffer, if any.
         * \return Index buffer or null.
         */
        [[nodiscard]] const IndexBufferPtr& getIndexBuffer() const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief UUID.
         */
        uuids::uuid uuid;

        /**
         * \brief List of one or more vertex buffers.
         */
        std::vector<VertexBufferPtr> vertexBuffers;

        /**
         * \brief Optional index buffer.
         */
        IndexBufferPtr indexBuffer;
    };
}  // namespace sol
