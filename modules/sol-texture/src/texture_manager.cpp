#include "sol-texture/texture_manager.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-error/sol_error.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/image2d.h"
#include "sol-texture/texture2d.h"
#include "sol-texture/image_transfer/improved_image_transfer.h"
#include "sol-texture/image_transfer/i_image_transfer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TextureManager::TextureManager(MemoryManager& memManager) :
        memoryManager(&memManager), imageTransfer(std::make_unique<ImprovedImageTransfer>(memManager))
    {
    }

    TextureManager::TextureManager(MemoryManager& memManager, IImageTransferPtr imgTransfer) :
        memoryManager(&memManager), imageTransfer(std::move(imgTransfer))
    {
    }

    TextureManager::~TextureManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MemoryManager& TextureManager::getMemoryManager() noexcept { return *memoryManager; }

    const MemoryManager& TextureManager::getMemoryManager() const noexcept { return *memoryManager; }

    ////////////////////////////////////////////////////////////////
    // Images.
    ////////////////////////////////////////////////////////////////

    Image2D& TextureManager::createImage2D(const VkFormat format, const std::array<uint32_t, 2>& size)
    {
        return createImage2D(format, size, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    }

    Image2D& TextureManager::createImage2D(const VkFormat                 format,
                                           const std::array<uint32_t, 2>& size,
                                           const VkImageUsageFlags        usage)
    {
        return createImage2DImpl(format, size, usage);
    }

    Image2D&
      TextureManager::createImage2DFill(const VkFormat format, const std::array<uint32_t, 2>& size, const void* value)
    {
        return createImage2DFill(format, size, value, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    }

    Image2D& TextureManager::createImage2DFill(const VkFormat                 format,
                                               const std::array<uint32_t, 2>& size,
                                               const void*                    value,
                                               const VkImageUsageFlags        usage)
    {
        auto& image2D = createImage2D(format, size, usage);
        image2D.createStagingBuffer();
        image2D.fill(value, 0);
        return image2D;
    }

    Image2D& TextureManager::createImage2D(const VkFormat format, const std::array<uint32_t, 2>& size, const void* data)
    {
        return createImage2D(format, size, data, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    }

    Image2D& TextureManager::createImage2D(const VkFormat                 format,
                                           const std::array<uint32_t, 2>& size,
                                           const void*                    data,
                                           const VkImageUsageFlags        usage)
    {
        auto& image2D = createImage2D(format, size, usage);
        image2D.createStagingBuffer();
        image2D.setData(data, 0);
        return image2D;
    }

    void TextureManager::destroyImage2D(Image2D& image)
    {
        // TODO: Do something with already staged transfers.
        if (&image.getTextureManager() != this)
            throw SolError("Cannot destroy image that is part of a different manager.");

        const auto it = std::ranges::find_if(images2D, [&](const Image2DPtr& i) { return i.get() == &image; });
        assert(it != images2D.end());
        images2D.erase(it);
    }

    ////////////////////////////////////////////////////////////////
    // Textures.
    ////////////////////////////////////////////////////////////////

    Texture2D& TextureManager::createTexture2D(Image2D& image2D)
    {
        auto texture2D = std::make_unique<Texture2D>(*this, image2D);
        textures2D.emplace_back(std::move(texture2D));
        return *textures2D.back();
    }

    Texture2D& TextureManager::createTexture2D(Image2D& image2D, VulkanImageViewPtr imageView, VulkanSamplerPtr sampler)
    {
        auto texture2D = std::make_unique<Texture2D>(*this, image2D, std::move(imageView), std::move(sampler));
        textures2D.emplace_back(std::move(texture2D));
        return *textures2D.back();
    }

    void TextureManager::destroyTexture2D(Texture2D& texture)
    {
        if (&texture.getTextureManager() != this)
            throw SolError("Cannot destroy texture that is part of a different manager.");

        const auto it = std::ranges::find_if(textures2D, [&](const Texture2DPtr& i) { return i.get() == &texture; });
        assert(it != textures2D.end());
        textures2D.erase(it);
    }

    ////////////////////////////////////////////////////////////////
    // Transfer.
    ////////////////////////////////////////////////////////////////

    size_t TextureManager::createStagingBuffer(Image2D&                       image,
                                               const std::array<uint32_t, 2>& regionOffset,
                                               const std::array<uint32_t, 2>& regionSize) const
    {
        if (&image.getTextureManager() != this) throw SolError("");

        return imageTransfer->createStagingBuffer(image, regionOffset, regionSize);
    }

    ImageStagingBuffer TextureManager::getStagingBuffer(Image2D& image, const size_t index) const
    {
        if (&image.getTextureManager() != this) throw SolError("");

        return imageTransfer->getStagingBuffer(image, index);
    }

    void TextureManager::stageTransition(Image2D& image) const { imageTransfer->stageTransition(image); }


    void TextureManager::transfer() const { imageTransfer->transfer(); }

    Image2D& TextureManager::createImage2DImpl(VkFormat                       format,
                                               const std::array<uint32_t, 2>& size,
                                               const VkImageUsageFlags        usage)
    {
        auto image2D = std::make_unique<Image2D>(*this, format, size, usage);

        // Stage a default transition. Reading in a shader on the graphics queue is the most common operation.
        image2D->stageTransition(&memoryManager->getGraphicsQueue().getFamily(),
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 VK_PIPELINE_STAGE_2_NONE,
                                 VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                                 VK_ACCESS_2_NONE,
                                 VK_ACCESS_2_SHADER_READ_BIT);

        images2D.emplace_back(std::move(image2D));
        return *images2D.back();
    }
}  // namespace sol
