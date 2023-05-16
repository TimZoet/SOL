#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"
#include "sol-texture/image_transfer/image_staging_buffer.h"

namespace sol
{
    class IImageTransfer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IImageTransfer() = default;

        IImageTransfer(const IImageTransfer&) = delete;

        IImageTransfer(IImageTransfer&&) = delete;

        virtual ~IImageTransfer() noexcept;

        IImageTransfer& operator=(const IImageTransfer&) = delete;

        IImageTransfer& operator=(IImageTransfer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Transfer.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual size_t createStagingBuffer(Image2D&                       image,
                                                         const std::array<uint32_t, 2>& regionOffset,
                                                         const std::array<uint32_t, 2>& regionSize) = 0;

        [[nodiscard]] virtual ImageStagingBuffer getStagingBuffer(Image2D& image, size_t index) = 0;

        virtual void stageTransition(Image2D&                     image,
                                     const VulkanQueueFamily*     queueFamily,
                                     std::optional<VkImageLayout> imageLayout,
                                     VkPipelineStageFlags2        srcStage,
                                     VkPipelineStageFlags2        dstStage,
                                     VkAccessFlags2               srcAccess,
                                     VkAccessFlags2               dstAccess) = 0;


        virtual void transfer() = 0;
    };
}  // namespace sol
