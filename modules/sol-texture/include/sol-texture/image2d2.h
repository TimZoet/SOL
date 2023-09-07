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

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

namespace sol
{
    class Image2D2 : public IImage
    {
    public:
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

        [[nodiscard]] const VulkanQueueFamily& getQueueFamily(uint32_t level, uint32_t layer) const noexcept override;

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
        [[nodiscard]] VkImageLayout getImageLayout(uint32_t level, uint32_t layer) const noexcept override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the queue family that owns the specified level and layer of this image.
         * \param family New queue family.
         * \param level Mip level to set owner for. If -1, set for all levels.
         * \param layer Array layer to set owner for. If -1, set for all layers.
         */
        void setQueueFamily(const VulkanQueueFamily& family, uint32_t level, uint32_t layer) noexcept override;

        /**
         * \brief Set the layout of the image for the the specified level and layer of this image.
         * \param layout Layout.
         * \param level Mip level to set layout for. If -1, set for all levels.
         * \param layer Array layer to set layout for. If -1, set for all layers.
         */
        void setImageLayout(VkImageLayout layout, uint32_t level, uint32_t layer) noexcept override;

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
    };
}  // namespace sol
