#include "sol-memory/i_image.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IImage::IImage(MemoryManager& memoryManager, uuids::uuid id) : manager(&memoryManager), uuid(id) {}

    IImage::~IImage() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& IImage::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& IImage::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& IImage::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& IImage::getMemoryManager() const noexcept { return *manager; }

    const uuids::uuid& IImage::getUuid() const noexcept { return uuid; }

    uint32_t IImage::getWidth() const noexcept { return getSize()[0]; }

    uint32_t IImage::getHeight() const noexcept { return getSize()[1]; }

    uint32_t IImage::getDepth() const noexcept { return getSize()[2]; }

    VkSubresourceLayout IImage::getSubresourceLayout(const uint32_t level, const uint32_t layer) const
    {
        if (getImageTiling() != VK_IMAGE_TILING_LINEAR)
            throw SolError("Cannot get subresource layout: image is not using linear tiling.");
        if (level >= getLevelCount())
            throw SolError(std::format(
              "Cannot get subresource layout of level {}: image only has {} levels.", level, getLevelCount()));
        if (layer >= getLayerCount())
            throw SolError(std::format(
              "Cannot get subresource layout of layer {}: image only has {} layers.", layer, getLayerCount()));

        const VkImageSubresource subresource{
          .aspectMask = getImageAspectFlags(), .mipLevel = level, .arrayLayer = layer};
        VkSubresourceLayout layout{};
        vkGetImageSubresourceLayout(getDevice().get(), getImage().get(), &subresource, &layout);
        return layout;
    }
}  // namespace sol
