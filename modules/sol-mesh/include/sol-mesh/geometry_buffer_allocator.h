#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/i_buffer_allocator.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class GeometryBufferAllocator final : public IBufferAllocator
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Allocation strategy.
         */
        enum class Strategy : uint32_t
        {
            /**
             * \brief Allocate a separate buffer for each vertex/index buffer.
             */
            Separate = 0,

            /**
             * \brief Preallocate a global vertex and index buffer with a fixed size
             * and return suballocations from these global buffers.
             */
            Global = 1
        };

        struct Settings
        {
            MemoryManager& memoryManager;

            /**
             * \brief Allocation strategy.
             */
            Strategy strategy = Strategy::Separate;

            /**
             * \brief Number of vertices. Only used when strategy == Global.
             */
            size_t vertexCount = 0;

            /**
             * \brief Vertex size in bytes. Only used when strategy == Global.
             */
            size_t vertexSize = 0;

            /**
             * \brief Number of indices. Only used when strategy == Global.
             */
            size_t indexCount = 0;

            /**
             * \brief Vertex size in bytes. Only used when strategy == Global.
             */
            size_t indexSize = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GeometryBufferAllocator() = delete;

        explicit GeometryBufferAllocator(MemoryManager& memoryManager);

        GeometryBufferAllocator(
          MemoryManager& memoryManager, IBufferPtr vtxBuffer, IBufferPtr idxBuffer, size_t vtxSize, size_t idxSize);

        [[nodiscard]] static GeometryBufferAllocatorPtr create(Settings settings);

        GeometryBufferAllocator(const GeometryBufferAllocator&) = delete;

        GeometryBufferAllocator(GeometryBufferAllocator&&) = delete;

        ~GeometryBufferAllocator() noexcept override;

        GeometryBufferAllocator& operator=(const GeometryBufferAllocator&) = delete;

        GeometryBufferAllocator& operator=(GeometryBufferAllocator&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Capabilities getCapabilities() const noexcept override;

        [[nodiscard]] Strategy getStrategy() const noexcept;

        [[nodiscard]] VmaVirtualBlock getVirtualVertexBlock() const noexcept;

        [[nodiscard]] VmaVirtualBlock getVirtualIndexBlock() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////////////
        // Allocations.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Allocate a new vertex buffer.
         * \param count Number of vertices.
         * \param size Size of each vertex in bytes. Ignored if strategy == Global.
         * \return Vertex buffer.
         */
        [[nodiscard]] VertexBufferPtr allocateVertexBuffer(size_t count, size_t size = 0);

        /**
         * \brief Allocate a new index buffer.
         * \param count Number of indices.
         * \param size Size of each index in bytes. Ignored if strategy == Global.
         * \return Index buffer.
         */
        [[nodiscard]] IndexBufferPtr allocateIndexBuffer(size_t count, size_t size = 0);

    protected:
        [[nodiscard]] IBufferPtr allocateBufferImpl(const AllocationInfo& alloc,
                                                    OnAllocationFailure   onFailure) override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
        * \brief Optional global vertex buffer.
        */
        IBufferPtr vertexBuffer;

        /**
         * \brief Optional global index buffer.
         */
        IBufferPtr indexBuffer;

        /**
         * \brief Size of each vertex in the global vertex buffer.
         */
        size_t vertexSize = 0;

        /**
         * \brief Size of each index in the global index buffer.
         */
        size_t indexSize = 0;

        /**
         * \brief Virtual block for the global vertex buffer.
         */
        VmaVirtualBlock virtualVertexBlock = VK_NULL_HANDLE;

        /**
         * \brief Virtual block for the global index buffer.
         */
        VmaVirtualBlock virtualIndexBlock = VK_NULL_HANDLE;
    };
}  // namespace sol
