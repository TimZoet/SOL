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
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

namespace sol
{
    class Image2D
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Transition
        {
            const VulkanQueueFamily*     targetFamily = nullptr;
            std::optional<VkImageLayout> newLayout    = {};
            VkPipelineStageFlags2        srcStage     = VK_PIPELINE_STAGE_2_NONE;
            VkPipelineStageFlags2        dstStage     = VK_PIPELINE_STAGE_2_NONE;
            VkAccessFlags2               srcAccess    = VK_ACCESS_2_NONE;
            VkAccessFlags2               dstAccess    = VK_ACCESS_2_NONE;
        };

        struct BarrierPair
        {
            std::optional<VkImageMemoryBarrier2> release;
            std::optional<VkImageMemoryBarrier2> acquire;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Image2D() = default;

        Image2D(TextureManager& manager, VkFormat vkFormat, std::array<uint32_t, 2> imgSize, VkImageUsageFlags vkUsage);

        Image2D(const Image2D&) = delete;

        Image2D(Image2D&&) = delete;

        ~Image2D() noexcept;

        Image2D& operator=(const Image2D&) = delete;

        Image2D& operator=(Image2D&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] TextureManager& getTextureManager() noexcept;

        [[nodiscard]] const TextureManager& getTextureManager() const noexcept;

        /**
         * \brief Get the Vulkan image managed by this object.
         * \return VulkanImage.
         */
        [[nodiscard]] VulkanImage& getImage() noexcept;

        /**
         * \brief Get the Vulkan image managed by this object.
         * \return VulkanImage.
         */
        [[nodiscard]] const VulkanImage& getImage() const noexcept;

        /**
         * \brief Get the image format.
         * \return Image format.
         */
        [[nodiscard]] VkFormat getFormat() const noexcept;

        /**
         * \brief Get the image size (resolution) in pixels.
         * \return Size.
         */
        [[nodiscard]] std::array<uint32_t, 2> getSize() const noexcept;

        /**
         * \brief Get the image width in pixels.
         * \return Width.
         */
        [[nodiscard]] uint32_t getWidth() const noexcept;

        /**
         * \brief Get the image height in pixels.
         * \return Height.
         */
        [[nodiscard]] uint32_t getHeight() const noexcept;

        /**
         * \brief Get the image usage flags.
         * \return ImageUsageFlags.
         */
        [[nodiscard]] VkImageUsageFlags getImageUsageFlags() const noexcept;

        /**
         * \brief Get the image aspect flags.
         * \return ImageAspectFlags.
         */
        [[nodiscard]] VkImageAspectFlags getAspectFlags() const noexcept;

        /**
         * \brief Get the queue family that currently owns this resource.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily* getQueueFamily() const noexcept;

        /**
         * \brief Get the current layout of the image.
         * \return VkImageLayout.
         */
        [[nodiscard]] VkImageLayout getImageLayout() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Transitions.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Stage a queue family ownership transfer and/or image layout transition.
         * Will override any existing transition that was not yet applied.
         * \param family Target queue family.
         * \param layout Optional new layout.
         * \param srcStage Source pipeline stage.
         * \param dstStage Destination pipeline stage.
         * \param srcAccess Source access flags.
         * \param dstAccess Destination access flags.
         */
        void  stageTransition(const VulkanQueueFamily*     family,
                              std::optional<VkImageLayout> layout,
                              VkPipelineStageFlags2        srcStage,
                              VkPipelineStageFlags2        dstStage,
                              VkAccessFlags2               srcAccess,
                              VkAccessFlags2               dstAccess);

        /**
         * \brief Get the currently staged transition.
         * \return Staged transition or empty.
         */
        [[nodiscard]] std::optional<Transition> getStagedTransition() const noexcept;

        /**
         * \brief Generate the pair of release and acquire barriers that transition this image
         * from its current state to its target state. This does not change any state of this
         * object. For that, call applyTransition().
         * \return Pair of release/acquire barriers.
         */
        [[nodiscard]] BarrierPair getTransitionBarriers() const;

        /**
         * \brief Apply staged transition. Does not have any effect if there is no staged transition.
         * Does not actually run any commands but merely updates the state of this object to reflect
         * that the transition was applied elsewhere.
         */
        void applyTransition();

        ////////////////////////////////////////////////////////////////
        // Data.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a staging buffer for the whole image.
         * \return Index of the staging buffer.
         */
        size_t createStagingBuffer();

        /**
         * \brief Create a staging buffer for a region of the image.
         * \param regionOffset Offset of the region (in pixels).
         * \param regionSize Size of the region (in pixels).
         * \return Index of the staging buffer
         */
        size_t createStagingBuffer(const std::array<uint32_t, 2>& regionOffset,
                                   const std::array<uint32_t, 2>& regionSize);

        /**
         * \brief Fill the image with the given data.
         * \param data Data array. Should contain at least as much data as the size of the image buffer.
         * \param stagingBufferIndex Index of the staging buffer.
         */
        void setData(const void* data, size_t stagingBufferIndex);

        /**
         * \brief Fill a region of the image with the given data.
         * \param data Data array. Should contain at least as much data as the size of a single pixel times the region area.
         * \param regionOffset Offset of the region to fill.
         * \param regionSize Size of the region to fill.
         * \param stagingBufferIndex Index of the staging buffer.
         */
        void setData(const void*                    data,
                     const std::array<uint32_t, 2>& regionOffset,
                     const std::array<uint32_t, 2>& regionSize,
                     size_t                         stagingBufferIndex);

        /**
         * \brief Fill the entire image with the given value.
         * \param value Value. Size should be equal to the size of a single pixel value.
         * \param stagingBufferIndex Index of the staging buffer.
         */
        void fill(const void* value, size_t stagingBufferIndex);

        /**
         * \brief Fill a region of the image with the given value.
         * \param value Value. Size should be equal to the size of a single pixel value.
         * \param regionOffset Offset of the region to fill.
         * \param regionSize Size of the region to fill.
         * \param stagingBufferIndex Index of the staging buffer.
         */
        void fill(const void*                    value,
                  const std::array<uint32_t, 2>& regionOffset,
                  const std::array<uint32_t, 2>& regionSize,
                  size_t                         stagingBufferIndex);

    private:
        ////////////////////////////////////////////////////////////////
        // Utility methods.
        ////////////////////////////////////////////////////////////////

        void initialize();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Manager of this image.
         */
        TextureManager* textureManager = nullptr;

        /**
         * \brief Image.
         */
        VulkanImagePtr image;

        /**
         * \brief Image format.
         */
        VkFormat format = VK_FORMAT_UNDEFINED;

        /**
         * \brief Image size (resolution).
         */
        std::array<uint32_t, 2> size = {0, 0};

        /**
         * \brief Image usage.
         */
        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        /**
         * \brief Image aspect.
         */
        VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

        /**
         * \brief Queue family that currently owns this image.
         */
        const VulkanQueueFamily* queueFamily = nullptr;

        /**
         * \brief Current image layout.
         */
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        std::optional<Transition> stagedTransition;
    };
}  // namespace sol
