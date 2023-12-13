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
        friend class MeshCollection;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Mesh();

        Mesh(MeshCollection& collection, uuids::uuid id);

        Mesh(const Mesh&) = delete;

        Mesh(Mesh&&) = delete;

        ~Mesh() noexcept;

        Mesh& operator=(const Mesh&) = delete;

        Mesh& operator=(Mesh&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the mesh collection this image is in.
         * \return MeshCollection.
         */
        [[nodiscard]] MeshCollection& getMeshCollection() noexcept;

        /**
         * \brief Get the mesh collection this image is in.
         * \return MeshCollection.
         */
        [[nodiscard]] const MeshCollection& getMeshCollection() const noexcept;

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
        [[nodiscard]] const std::vector<VertexBufferPtr>& getVertexBuffers() const noexcept;

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
         * \brief Mesh collection this image is in.
         */
        MeshCollection* meshCollection = nullptr;

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
