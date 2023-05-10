#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"
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
    class IMesh
    {
    public:
        enum class MeshType
        {
            Flat,
            Indexed,
            Multi,
            Shared,
            Other
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IMesh();

        IMesh(MeshManager& manager, uuids::uuid id);

        IMesh(const IMesh&) = delete;

        IMesh(IMesh&&) = delete;

        virtual ~IMesh() noexcept;

        IMesh& operator=(const IMesh&) = delete;

        IMesh& operator=(IMesh&&) = delete;

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
         * \brief Get the UUID.
         * \return UUID.
         */
        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        /**
         * \brief Get the user friendly name.
         * \return Name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        /**
         * \brief Get the queue family that currently owns this resource.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily* getCurrentFamily() const noexcept;

        /**
         * \brief Get the queue family that should own this resource after the next data and/or ownership transfer.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily* getTargetFamily() const noexcept;
        
        [[nodiscard]] VkPipelineStageFlags getStageFlags() const noexcept;

        /**
         * \brief Get the mesh type.
         * \return MeshType.
         */
        [[nodiscard]] virtual MeshType getType() const noexcept = 0;

        [[nodiscard]] virtual bool isValid() const noexcept = 0;

        /**
         * \brief Returns whether this mesh is indexed.
         * \return True if indexed, false otherwise.
         */
        [[nodiscard]] virtual bool isIndexed() const noexcept = 0;

        /**
         * \brief Get the handles of the vertex buffers that should be bound (passed to vkCmdBindVertexBuffers).
         * \param handles List to append handles to.
         */
        virtual void getVertexBufferHandles(std::vector<VkBuffer>& handles) const = 0;

        /**
          * \brief Get the list of offsets with which the vertex buffers are bound (passed to vkCmdBindVertexBuffers).
          * \param offsets List to append offsets to.
          */
        virtual void getVertexBufferOffsets(std::vector<size_t>& offsets) const = 0;

        /**
         * \brief Get the number of vertex buffers in this mesh.
         * \return Number of vertex buffers.
         */
        [[nodiscard]] virtual size_t getVertexBufferCount() const noexcept = 0;

        /**
         * \brief Get the handle of the index buffer that should be bound during rendering (passed to vkCmdBindIndexBuffer).
         * \return VkBuffer.
         */
        [[nodiscard]] virtual VkBuffer getIndexBufferHandle() const noexcept = 0;

        /**
          * \brief Get the offset in bytes with which the index buffer is bound (passed to vkCmdBindIndexBuffer).
          * \return Offset in bytes.
          */
        [[nodiscard]] virtual size_t getIndexBufferOffset() const noexcept = 0;

        /**
         * \brief Get the index type (passed to vkCmdDrawIndexed, only used for indexed meshes).
         * \return VkIndexType.
         */
        [[nodiscard]] virtual VkIndexType getIndexType() const noexcept = 0;

        /**
         * \brief Get the number of vertices to draw (passed to vkCmdDraw, only used for non-indexed meshes).
         * \return Vertex count.
         */
        [[nodiscard]] virtual uint32_t getVertexCount() const noexcept = 0;

        /**
         * \brief Get the index of the first vertex (passed to vkCmdDraw, only used for non-indexed meshes).
         * \return Index.
         */
        [[nodiscard]] virtual uint32_t getFirstVertex() const noexcept = 0;

        /**
         * \brief Get the number of indices to draw (passed to vkCmdDrawIndexed, only used for indexed meshes).
         * \return Index count.
         */
        [[nodiscard]] virtual uint32_t getIndexCount() const noexcept = 0;

        /**
         * \brief Get the base index within the index buffer (passed to vkCmdDrawIndexed, only used for indexed meshes).
         * \return Index.
         */
        [[nodiscard]] virtual uint32_t getFirstIndex() const noexcept = 0;

        /**
         * \brief Get the offset that is added to indices read from the index buffer (passed to vkCmdDrawIndexed, only used for indexed meshes).
         * \return Offset.
         */
        [[nodiscard]] virtual int32_t getVertexOffset() const noexcept = 0;

        /**
         * \brief For each vertex buffer of this mesh, the access flags of that buffer.
         * \return List of VkAccessFlags.
         */
        [[nodiscard]] virtual std::vector<VkAccessFlags> getVertexBufferAccessFlags() const noexcept = 0;

        /**
         * \brief If this mesh has an index buffer, the access flags of that buffer.
         * \return VkAccessFlags.
         */
        [[nodiscard]] virtual VkAccessFlags getIndexBufferAccessFlags() const noexcept = 0;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the user friendly name.
         * \param value Name.
         */
        void setName(std::string value);

        void setCurrentFamily(const VulkanQueueFamily& family);

        void setTargetFamily(const VulkanQueueFamily& family);

        void setStageFlags(VkPipelineStageFlags flags);

        ////////////////////////////////////////////////////////////////
        // Update.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Update the mesh buffers with the data from the given mesh description.
         * \param desc MeshDescription to update from.
         */
        virtual void update(MeshDescriptionPtr desc);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief MeshManager.
         */
        MeshManager* meshManager = nullptr;

        /**
         * \brief UUID.
         */
        uuids::uuid uuid;

        /**
         * \brief User friendly name.
         */
        std::string name;

        /**
         * \brief Queue family that currently owns this mesh.
         */
        const VulkanQueueFamily* currentFamily = nullptr;

        /**
         * \brief Queue family that should own this mesh after an ownership transfer.
         */
        const VulkanQueueFamily* targetFamily = nullptr;

        /**
         * \brief Stages at which this mesh is accessed.
         */
        VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    };
}  // namespace sol
