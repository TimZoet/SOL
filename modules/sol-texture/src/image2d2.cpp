#include "sol-texture/image2d2.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image.h"

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

    const VulkanQueueFamily& Image2D2::getQueueFamily(const uint32_t level, const uint32_t layer) const noexcept
    {
        assert(level < queueFamily.size());
        assert(layer == 0);
        return *queueFamily[level];
    }

    VulkanImage& Image2D2::getImage() noexcept { return *image; }

    const VulkanImage& Image2D2::getImage() const noexcept { return *image; }

    IImage::ImageType Image2D2::getImageType() const noexcept { return ImageType::Image2D; }

    uint32_t Image2D2::getLevelCount() const noexcept { return static_cast<uint32_t>(imageLayout.size()); }

    uint32_t Image2D2::getLayerCount() const noexcept { return 1; }

    std::array<uint32_t, 3> Image2D2::getSize() const noexcept { return {size[0], size[0], 1}; }

    VkFormat Image2D2::getFormat() const noexcept { return format; }

    VkImageUsageFlags Image2D2::getImageUsageFlags() const noexcept { return usageFlags; }

    VkImageAspectFlags Image2D2::getImageAspectFlags() const noexcept { return aspectFlags; }

    VkImageLayout Image2D2::getImageLayout(const uint32_t level, const uint32_t layer) const noexcept
    {
        assert(level < queueFamily.size());
        assert(layer == 0);
        return imageLayout[level];
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Image2D2::setQueueFamily(const VulkanQueueFamily& family, const uint32_t level, const uint32_t layer) noexcept
    {
        assert(level < queueFamily.size() || level == static_cast<uint32_t>(-1));
        assert(layer == 0 || layer == static_cast<uint32_t>(-1));

        if (level == static_cast<uint32_t>(-1) || queueFamily.size() == 1)
            std::ranges::fill(queueFamily, &family);
        else
            queueFamily[level] = &family;
    }

    void Image2D2::setImageLayout(const VkImageLayout layout, const uint32_t level, const uint32_t layer) noexcept
    {
        assert(level < queueFamily.size());
        assert(layer == 0);

        if (level == static_cast<uint32_t>(-1) || imageLayout.size() == 1)
            std::ranges::fill(imageLayout, layout);
        else
            imageLayout[level] = layout;
    }
}  // namespace sol
