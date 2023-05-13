#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"
#include "sol-mesh/mesh_transfer/i_mesh_transfer.h"

namespace sol
{
    class DefaultMeshTransfer final : public IMeshTransfer
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct StagedCopy
        {
            MeshDescriptionPtr meshDescription;

            IMesh* mesh = nullptr;
        };

        struct AcquireState
        {
            /**
             * \brief Command buffer for each queue family to release ownership to transfer queue.
             */
            std::vector<VulkanCommandBufferPtr> releaseCommandBuffers;

            /**
             * \brief Command buffer for each queue family to acquire ownership on transfer queue.
             */
            std::vector<VulkanCommandBufferPtr> acquireCommandBuffers;

            /**
             * \brief Semaphore to synchronize release and acquire for each queue family.
             */
            std::vector<VulkanSemaphorePtr> semaphores;

            /**
             * \brief Fence for acquires.
             */
            VulkanFencePtr fence;
        };

        struct CopyState
        {
            /**
             * \brief Command buffer for copying.
             */
            VulkanCommandBufferPtr commandBuffer;

            /**
             * \brief Fence for copy.
             */
            VulkanFencePtr fence;
        };

        struct ReleaseState
        {
            /**
             * \brief Command buffer for each queue family to release ownership to target queue.
             */
            std::vector<VulkanCommandBufferPtr> releaseCommandBuffers;

            /**
             * \brief Command buffer for each queue family to acquire ownership on target queue.
             */
            std::vector<VulkanCommandBufferPtr> acquireCommandBuffers;

            /**
             * \brief Semaphore to synchronize release and acquire for each queue family.
             */
            std::vector<VulkanSemaphorePtr> semaphores;

            /**
             * \brief Fence for each queue family.
             */
            std::vector<VulkanFencePtr> fences;
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        DefaultMeshTransfer() = default;

        explicit DefaultMeshTransfer(MeshManager& manager);

        DefaultMeshTransfer(const DefaultMeshTransfer&) = delete;

        DefaultMeshTransfer(DefaultMeshTransfer&&) = delete;

        ~DefaultMeshTransfer() noexcept override;

        DefaultMeshTransfer& operator=(const DefaultMeshTransfer&) = delete;

        DefaultMeshTransfer& operator=(DefaultMeshTransfer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Transfer.
        ////////////////////////////////////////////////////////////////

       void stageCopy(MeshDescriptionPtr meshDescription, IMesh& mesh) override;

        void transfer() override;

    private:
        /**
         * \brief Acquire ownership of buffers on transfer queue.
         */
        void acquireOwnership();

        /**
         * \brief Perform actual memory copies.
         */
        void copyBuffers();

        /**
         * \brief Release ownership of buffers to target queue.
         */
        void releaseOwnership();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        std::vector<StagedCopy> stagedCopies;

        std::vector<StagedCopy> pendingCopies;

        AcquireState acquireState;

        CopyState copyState;

        ReleaseState releaseState;
    };
}  // namespace sol