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
    class ImprovedImageTransfer final : public IImageTransfer
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct TransitionInfo
        {
            VkPipelineStageFlags2    srcStage  = VK_PIPELINE_STAGE_2_NONE;
            VkPipelineStageFlags2    dstStage  = VK_PIPELINE_STAGE_2_NONE;
            VkAccessFlags2           srcAccess = VK_ACCESS_2_NONE;
            VkAccessFlags2           dstAccess = VK_ACCESS_2_NONE;
            VkImageLayout            oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkImageLayout            newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            const VulkanQueueFamily* srcFamily = nullptr;
            const VulkanQueueFamily* dstFamily = nullptr;
        };

        struct CopyInfo
        {
            VulkanBufferPtr         buffer;
            std::array<uint32_t, 2> regionOffset;
            std::array<uint32_t, 2> regionSize;
        };

        struct TransferInfo
        {
            TransitionInfo     transition;
            VkImage            image  = VK_NULL_HANDLE;
            VkImageAspectFlags aspect = VK_IMAGE_ASPECT_NONE;

            /**
             * \brief List of 0 or more staged copies. If empty, only a transition needs to be performed.
             */
            std::vector<CopyInfo> copies;
        };

        struct QueuePair
        {
            const VulkanQueueFamily* srcFamily = nullptr;
            const VulkanQueueFamily* dstFamily = nullptr;
            VulkanSemaphorePtr       sem;
            VulkanFencePtr           fence;
            VulkanCommandBufferPtr   srcCommandBuffer;
            VulkanCommandBufferPtr   dstCommandBuffer;
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ImprovedImageTransfer() = default;

        explicit ImprovedImageTransfer(MemoryManager& manager);

        ImprovedImageTransfer(const ImprovedImageTransfer&) = delete;

        ImprovedImageTransfer(ImprovedImageTransfer&&) = delete;

        ~ImprovedImageTransfer() noexcept override;

        ImprovedImageTransfer& operator=(const ImprovedImageTransfer&) = delete;

        ImprovedImageTransfer& operator=(ImprovedImageTransfer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Transfer.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] size_t createStagingBuffer(Image2D&                       image,
                                                 const std::array<uint32_t, 2>& regionOffset,
                                                 const std::array<uint32_t, 2>& regionSize) override;

        [[nodiscard]] ImageStagingBuffer getStagingBuffer(Image2D& image, size_t index) override;

        void stageTransition(Image2D&                     image,
                             const VulkanQueueFamily*     queueFamily,
                             std::optional<VkImageLayout> imageLayout,
                             VkPipelineStageFlags2        srcStage,
                             VkPipelineStageFlags2        dstStage,
                             VkAccessFlags2               srcAccess,
                             VkAccessFlags2               dstAccess) override;

        void transfer() override;

    private:
        /**
         * \brief Acquire ownership of images that have staged copies on transfer queue.
         */
        void prepareCopy();

        /**
         * \brief Perform actual memory copies.
         */
        void copyImages();

        /**
         * \brief Perform layout transitions and acquire ownership on destination queues.
         */
        void transition();

        /**
         * \brief Wait for pending copies to complete.
         */
        void wait();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        std::unordered_map<Image2D*, TransferInfo> staged;

        std::unordered_map<Image2D*, TransferInfo> pending;

        /**
         * \brief List of command buffers, one for each queue family, to release ownership to the transfer queue.
         */
        std::vector<VulkanCommandBufferPtr> releaseCommandBuffers;

        /**
         * \brief List of command buffers, one for each queue family, to acquire ownership on the transfer queue.
         */
        std::vector<VulkanCommandBufferPtr> acquireCommandBuffers;

        /**
         * \brief List of semaphores, one for each queue family, to synchronize above release-acquire command buffer pairs.
         */
        std::vector<VulkanSemaphorePtr> releaseSemaphores;

        /**
         * \brief List of fences, one for each queue family, to track status of above acquire command buffers.
         */
        std::vector<VulkanFencePtr> acquireFences;

        struct
        {
            VulkanCommandBufferPtr commandBuffer;

            VulkanFencePtr fence;
        } copy;

        /**
         * \brief Resources for transitions and transfers from one queue family to another queue family.
         * With N unique queue families, there are NxN possible combinations. As an example, if we have
         * a Compute, Graphics and Transfer family, the layout  of the list would be this:
         * [C=>C, G=>C, T=>C,
         *  C=>G, G=>G, T=>G,
         *  C=>T, G=>T, T=>T]
         */
        std::vector<QueuePair> commands;
    };
}  // namespace sol
