#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"
#include "sol-texture/image_transfer/i_image_transfer.h"

namespace sol
{
    class DefaultImageTransfer final : public IImageTransfer
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct StagingBuffer
        {
            /**
             * \brief Staging buffer.
             */
            VulkanBufferPtr stagingBuffer;

            /**
             * \brief Offset of the image region this staging buffer covers.
             */
            std::array<uint32_t, 2> regionOffset;

            /**
             * \brief Size of the image region this staging buffer covers.
             */
            std::array<uint32_t, 2> regionSize;
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

        DefaultImageTransfer() = default;

        explicit DefaultImageTransfer(MemoryManager& manager);

        DefaultImageTransfer(const DefaultImageTransfer&) = delete;

        DefaultImageTransfer(DefaultImageTransfer&&) = delete;

        ~DefaultImageTransfer() noexcept override;

        DefaultImageTransfer& operator=(const DefaultImageTransfer&) = delete;

        DefaultImageTransfer& operator=(DefaultImageTransfer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Transfer.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] size_t createStagingBuffer(Image2D&                       image,
                                                 const std::array<uint32_t, 2>& regionOffset,
                                                 const std::array<uint32_t, 2>& regionSize) override;

        [[nodiscard]] ImageStagingBuffer getStagingBuffer(Image2D& image, size_t index) override;

        void stageLayoutTransition(Image2D& image) override;

        void stageOwnershipTransfer(Image2D& image) override;

        void transfer() override;

    private:
        /**
         * \brief Acquire ownership of images on transfer queue.
         */
        void acquireOwnership();

        /**
         * \brief Perform actual memory copies.
         */
        void copyImages();

        /**
         * \brief Release ownership of images to target queue.
         */
        void releaseOwnership();

        /**
         * \brief Wait for pending copies to complete.
         */
        void wait();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        std::unordered_map<Image2D*, std::vector<StagingBuffer>> stagedCopies;

        std::unordered_map<Image2D*, std::vector<StagingBuffer>> pendingCopies;

        AcquireState acquireState;

        CopyState copyState;

        ReleaseState releaseState;
    };
}  // namespace sol