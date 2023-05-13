#pragma once

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
         * \brief Get the queue family that currently owns this resource.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily* getCurrentFamily() const noexcept;

        /**
         * \brief Get the queue family that should own this resource after the next data and/or ownership transfer.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily* getTargetFamily() const noexcept;

        [[nodiscard]] VkImageLayout getCurrentLayout() const noexcept;

        [[nodiscard]] VkImageLayout getTargetLayout() const noexcept;

        [[nodiscard]] VkPipelineStageFlags getStageFlags() const noexcept;

        [[nodiscard]] VkAccessFlags getAccessFlags() const noexcept;

        [[nodiscard]] VkImageAspectFlags getAspectFlags() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setCurrentFamily(const VulkanQueueFamily& family);

        void setTargetFamily(const VulkanQueueFamily& family);

        void setCurrentLayout(VkImageLayout layout);

        void setTargetLayout(VkImageLayout layout);

        void setStageFlags(VkPipelineStageFlags flags);

        void setAccessFlags(VkAccessFlags flags);

        void setAspectFlags(VkImageAspectFlags flags);

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
         * \brief Queue family that currently owns this image.
         */
        const VulkanQueueFamily* currentFamily = nullptr;

        /**
         * \brief Queue family that should own this image after an ownership transfer.
         */
        const VulkanQueueFamily* targetFamily = nullptr;

        /**
         * \brief Current image layout.
         */
        VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        /**
         * \brief Layout this image should have after a layout transition.
         */
        VkImageLayout targetLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        /**
         * \brief Stages at which this image is accessed.
         */
        VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        VkAccessFlags accessFlags = VK_ACCESS_SHADER_READ_BIT;

        VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    };
}  // namespace sol