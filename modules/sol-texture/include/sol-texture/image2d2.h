#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-memory/i_image.h"
#include "sol-memory/transaction.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

namespace sol
{
    class Image2D2 : public IImage
    {
    public:
        struct Barrier
        {
            /**
             * \brief Destination queue family. If not null, the image will be owned by dstFamily after the barrier. Otherwise, the owner will remain the same.
             */
            const VulkanQueueFamily* dstFamily = nullptr;

            /**
             * \brief Source stage.
             */
            VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_NONE_KHR;

            /**
             * \brief Destination stage.
             */
            VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_NONE_KHR;

            /**
             * \brief Source access.
             */
            VkAccessFlags2 srcAccess = VK_ACCESS_2_NONE;

            /**
             * \brief Destination access.
             */
            VkAccessFlags2 dstAccess = VK_ACCESS_2_NONE;

            /**
             * \brief Destination layout. Only used if not the same as srcLayout.
             */
            VkImageLayout dstLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        };

        struct CopyRegion
        {
            /**
             * \brief Offset into data array / buffer.
             */
            size_t dataOffset = 0;

            /**
             * \brief Mip level.
             */
            uint32_t level = 0;

            /**
             * \brief Offset of the region in pixels.
             */
            std::array<int32_t, 2> regionOffset = {0, 0};

            /**
             * \brief Size of the region in pixels. If 0, set to image size.
             */
            std::array<uint32_t, 2> regionSize = {0, 0};
        };

        friend class TextureCollection;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Image2D2() = delete;

        Image2D2(TextureCollection& collection, uuids::uuid id);

        Image2D2(const Image2D2&) = delete;

        Image2D2(Image2D2&&) = delete;

        ~Image2D2() noexcept override;

        Image2D2& operator=(const Image2D2&) = delete;

        Image2D2& operator=(Image2D2&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the texture collection this image is in.
         * \return TextureCollection.
         */
        [[nodiscard]] TextureCollection& getTextureCollection() noexcept;

        /**
         * \brief Get the texture collection this image is in.
         * \return TextureCollection.
         */
        [[nodiscard]] const TextureCollection& getTextureCollection() const noexcept;

        /**
         * \brief Get the queue family that currently owns this resource.
         * \param level Mip level to get owner for.
         * \param layer Array layer to get owner for.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily& getQueueFamily(uint32_t level, uint32_t layer) const override;

        /**
         * \brief Get the Vulkan image managed by this object.
         * \return VulkanImage.
         */
        [[nodiscard]] VulkanImage& getImage() noexcept override;

        /**
         * \brief Get the Vulkan image managed by this object.
         * \return VulkanImage.
         */
        [[nodiscard]] const VulkanImage& getImage() const noexcept override;

        /**
         * \brief Get the image type.
         * \return ImageType.
         */
        [[nodiscard]] ImageType getImageType() const noexcept override;

        /**
         * \brief Get the number of mip levels.
         * \return Level count.
         */
        [[nodiscard]] uint32_t getLevelCount() const noexcept override;

        /**
         * \brief Get the number of array layers.
         * \return Layer count.
         */
        [[nodiscard]] uint32_t getLayerCount() const noexcept override;

        /**
         * \brief Get the image size (resolution) in pixels.
         * \return Size.
         */
        [[nodiscard]] std::array<uint32_t, 3> getSize() const noexcept override;

        /**
         * \brief Get the image format.
         * \return Image format.
         */
        [[nodiscard]] VkFormat getFormat() const noexcept override;

        /**
         * \brief Get the image usage flags.
         * \return ImageUsageFlags.
         */
        [[nodiscard]] VkImageUsageFlags getImageUsageFlags() const noexcept override;

        /**
         * \brief Get the image aspect flags.
         * \return ImageAspectFlags.
         */
        [[nodiscard]] VkImageAspectFlags getImageAspectFlags() const noexcept override;

        /**
         * \brief Get the current layout of the image.
         * \param level Mip level to get layout for.
         * \param layer Array layer to get layout for.
         * \return VkImageLayout.
         */
        [[nodiscard]] VkImageLayout getImageLayout(uint32_t level, uint32_t layer) const override;

        /**
         * \brief Get the image tiling mode.
         * \return VkImageTiling
         */
        [[nodiscard]] VkImageTiling getImageTiling() const override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setQueueFamily(const VulkanQueueFamily& family, uint32_t level, uint32_t layer) override;

        void setImageLayout(VkImageLayout layout, uint32_t level, uint32_t layer) override;

        ////////////////////////////////////////////////////////////////
        // Transactions.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Place a barrier.
         * \param transaction Transaction to append to.
         * \param barrier Barrier.
         * \param location Where to place the barrier, if a separate copy on the same image is being staged.
         */
        void barrier(Transaction& transaction, const Barrier& barrier, BarrierLocation location);

        /**
         * \brief Set the image data for a list of regions. Can fail if there is not enough memory for a staging buffer.
         * \param transaction Transaction to append to.
         * \param data Pointer to data. Will be copied into a staging buffer. Can be released directly after this call.
         * \param dataSize Size of data in bytes. A staging buffer of this size is allocated.
         * \param barrier Barrier placed around the copy command. Transfer stage, access and layout are automatically taken care of.
         * \param waitOnAllocFailure If true and staging buffer allocation fails, wait on previous transaction(s)
         * to complete so that they release their staging buffers, and try allocating again.
         * \param regions List of regions.
         * \return True if staging buffer allocation succeeded and transaction can be committed.
         * On failure, already staged transactions should be committed and waited on before trying again.
         */
        [[nodiscard]] bool setData(Transaction&                   transaction,
                                   const void*                    data,
                                   size_t                         dataSize,
                                   const Barrier&                 barrier,
                                   bool                           waitOnAllocFailure,
                                   const std::vector<CopyRegion>& regions);

        /**
         * \brief Get the image data for a list of regions. Copies the data to a destination buffer.
         * \param transaction Transaction to append to.
         * \param dstBuffer Destination buffer.
         * \param srcBarrier Barrier placed around the copy command for the source image.
         * \param dstBarrier Barrier placed around the copy command for the destination buffer.
         * \param regions List of regions.
         */
        void getData(Transaction&                   transaction,
                     IBuffer&                       dstBuffer,
                     const Barrier&                 srcBarrier,
                     const Barrier&                 dstBarrier,
                     const std::vector<CopyRegion>& regions);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Texture collection this image is in.
         */
        TextureCollection* textureCollection = nullptr;

        /**
         * \brief Queue family that currently owns each mip level.
         */
        std::vector<const VulkanQueueFamily*> queueFamily;

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
        VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        /**
         * \brief Image aspect.
         */
        VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

        /**
         * \brief Current image layout for each mip level.
         */
        std::vector<VkImageLayout> imageLayout;

        /**
         * \brief Image tiling.
         */
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    };
}  // namespace sol
