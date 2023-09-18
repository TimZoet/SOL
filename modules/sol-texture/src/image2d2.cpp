#include "sol-texture/image2d2.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image.h"
#include "sol-error/sol_error.h"
#include "sol-memory/i_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/texture_collection.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Image2D2::Image2D2(TextureCollection& collection, const uuids::uuid id) :
        IImage(collection.getMemoryManager(), id), textureCollection(&collection)
    {
    }

    Image2D2::~Image2D2() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    TextureCollection& Image2D2::getTextureCollection() noexcept { return *textureCollection; }

    const TextureCollection& Image2D2::getTextureCollection() const noexcept { return *textureCollection; }

    const VulkanQueueFamily& Image2D2::getQueueFamily(const uint32_t level, const uint32_t layer) const
    {
        if (level >= queueFamily.size())
            throw SolError(
              std::format("Cannot get queue family of level {}: image only has {} levels.", level, queueFamily.size()));
        if (layer > 0)
            throw SolError(std::format("Cannot get queue family of layer {}: image is not an array image.", layer));

        return *queueFamily[level];
    }

    VulkanImage& Image2D2::getImage() noexcept { return *image; }

    const VulkanImage& Image2D2::getImage() const noexcept { return *image; }

    IImage::ImageType Image2D2::getImageType() const noexcept { return ImageType::Image2D; }

    uint32_t Image2D2::getLevelCount() const noexcept { return static_cast<uint32_t>(imageLayout.size()); }

    uint32_t Image2D2::getLayerCount() const noexcept { return 1; }

    std::array<uint32_t, 3> Image2D2::getSize() const noexcept { return {size[0], size[1], 1}; }

    VkFormat Image2D2::getFormat() const noexcept { return format; }

    VkImageUsageFlags Image2D2::getImageUsageFlags() const noexcept { return usageFlags; }

    VkImageAspectFlags Image2D2::getImageAspectFlags() const noexcept { return aspectFlags; }

    VkImageLayout Image2D2::getImageLayout(const uint32_t level, const uint32_t layer) const
    {
        if (level >= queueFamily.size())
            throw SolError(
              std::format("Cannot get image layout of level {}: image only has {} levels.", level, queueFamily.size()));
        if (layer > 0)
            throw SolError(std::format("Cannot get image layout of layer {}: image is not an array image.", layer));

        return imageLayout[level];
    }

    VkImageTiling Image2D2::getImageTiling() const { return tiling; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////
    // Transactions.
    ////////////////////////////////////////////////////////////////

    void Image2D2::barrier(Transaction& transaction, const Barrier& barrier, const BarrierLocation location)
    {
        // We just create a barrier for all levels and layers.
        const ImageBarrier imgBarrier{.image          = *this,
                                      .srcFamily      = queueFamily[0],
                                      .dstFamily      = barrier.dstFamily ? barrier.dstFamily : queueFamily[0],
                                      .srcStage       = barrier.srcStage,
                                      .dstStage       = barrier.dstStage,
                                      .srcAccess      = barrier.srcAccess,
                                      .dstAccess      = barrier.dstAccess,
                                      .srcLayout      = imageLayout[0],
                                      .dstLayout      = barrier.dstLayout,
                                      .aspectMask     = getImageAspectFlags(),
                                      .baseMipLevel   = 0,
                                      .levelCount     = getLevelCount(),
                                      .baseArrayLayer = 0,
                                      .layerCount     = getLayerCount()};

        // Update image layout and queue family for all levels and layers.
        if (imgBarrier.srcLayout != imgBarrier.dstLayout) std::ranges::fill(imageLayout, imgBarrier.dstLayout);
        if (imgBarrier.srcFamily != imgBarrier.dstFamily) std::ranges::fill(queueFamily, imgBarrier.dstFamily);

        transaction.stage(imgBarrier, location);
    }

    bool Image2D2::setData(Transaction&             transaction,
                           const void*                    data,
                           const size_t                   dataSize,
                           const Barrier&                 barrier,
                           const bool                     waitOnAllocFailure,
                           const std::vector<CopyRegion>& regions)
    {
        // Fill up copy with all regions.
        StagingImageCopy copy{.dstImage = *this, .data = data, .dataSize = dataSize, .regions = {}};
        for (const auto& [dataOffset, level, regionOffset, regionSize] : regions)
        {
            auto rSize = regionSize;
            if (rSize[0] == 0) rSize[0] = getWidth();
            if (rSize[1] == 0) rSize[1] = getHeight();
            copy.regions.emplace_back(dataOffset,
                                      getImageAspectFlags(),
                                      level,
                                      0,
                                      1,
                                      std::array{regionOffset[0], regionOffset[1], 0},
                                      std::array{rSize[0], rSize[1], 1u});
        }

        // We just create a barrier for all levels and layers.
        const ImageBarrier imgBarrier{.image          = *this,
                                      .srcFamily      = queueFamily[0],
                                      .dstFamily      = barrier.dstFamily ? barrier.dstFamily : queueFamily[0],
                                      .srcStage       = barrier.srcStage,
                                      .dstStage       = barrier.dstStage,
                                      .srcAccess      = barrier.srcAccess,
                                      .dstAccess      = barrier.dstAccess,
                                      .srcLayout      = imageLayout[0],
                                      .dstLayout      = barrier.dstLayout,
                                      .aspectMask     = getImageAspectFlags(),
                                      .baseMipLevel   = 0,
                                      .levelCount     = getLevelCount(),
                                      .baseArrayLayer = 0,
                                      .layerCount     = getLayerCount()};

        // Update image layout and queue family for all levels and layers.
        if (imgBarrier.srcLayout != imgBarrier.dstLayout) std::ranges::fill(imageLayout, imgBarrier.dstLayout);
        if (imgBarrier.srcFamily != imgBarrier.dstFamily) std::ranges::fill(queueFamily, imgBarrier.dstFamily);

        return transaction.stage(copy, imgBarrier, waitOnAllocFailure);
    }

    void Image2D2::getData(Transaction&             transaction,
                           IBuffer&                       dstBuffer,
                           const Barrier&                 srcBarrier,
                           const Barrier&                 dstBarrier,
                           const std::vector<CopyRegion>& regions)
    {
        // Fill up copy with all regions.
        ImageToBufferCopy copy{
          .srcImage = *this, .dstBuffer = dstBuffer, .regions = {}, .dstOnDedicatedTransfer = true};

        for (const auto& [dataOffset, level, regionOffset, regionSize] : regions)
        {
            auto rSize = regionSize;
            if (rSize[0] == 0) rSize[0] = getWidth();
            if (rSize[1] == 0) rSize[1] = getHeight();
            copy.regions.emplace_back(dataOffset,
                                      getImageAspectFlags(),
                                      level,
                                      0,
                                      1,
                                      std::array{regionOffset[0], regionOffset[1], 0},
                                      std::array{rSize[0], rSize[1], 1u});
        }

        // We just create a barrier for all levels and layers.
        const ImageBarrier imgBarrier{.image          = *this,
                                      .srcFamily      = queueFamily[0],
                                      .dstFamily      = srcBarrier.dstFamily ? srcBarrier.dstFamily : queueFamily[0],
                                      .srcStage       = srcBarrier.srcStage,
                                      .dstStage       = srcBarrier.dstStage,
                                      .srcAccess      = srcBarrier.srcAccess,
                                      .dstAccess      = srcBarrier.dstAccess,
                                      .srcLayout      = imageLayout[0],
                                      .dstLayout      = srcBarrier.dstLayout,
                                      .aspectMask     = getImageAspectFlags(),
                                      .baseMipLevel   = 0,
                                      .levelCount     = getLevelCount(),
                                      .baseArrayLayer = 0,
                                      .layerCount     = getLayerCount()};

        // Update image layout and queue family for all levels and layers.
        if (imgBarrier.srcLayout != imgBarrier.dstLayout) std::ranges::fill(imageLayout, imgBarrier.dstLayout);
        if (imgBarrier.srcFamily != imgBarrier.dstFamily) std::ranges::fill(queueFamily, imgBarrier.dstFamily);

        const BufferBarrier bufferBarrier{.buffer = dstBuffer,
                                          .dstFamily =
                                            dstBarrier.dstFamily ? dstBarrier.dstFamily : &dstBuffer.getQueueFamily(),
                                          .srcStage  = dstBarrier.srcStage,
                                          .dstStage  = dstBarrier.dstStage,
                                          .srcAccess = dstBarrier.srcAccess,
                                          .dstAccess = dstBarrier.dstAccess};

        // TODO: If transaction no longer automatically takes care of this, also set family of destination buffer.

        return transaction.stage(copy, imgBarrier, bufferBarrier);
    }

}  // namespace sol
