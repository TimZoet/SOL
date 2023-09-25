#include "sol-texture/texture_collection.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cmath>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

// TODO: This should not be necessary and fixed by the uuid lib.
#define NOMINMAX
#include <uuid_system_generator.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-error/sol_error.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/image2d2.h"
#include "sol-texture/sampler2d.h"
#include "sol-texture/texture2d2.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TextureCollection::TextureCollection(MemoryManager& manager) : memoryManager(&manager) {}

    TextureCollection::~TextureCollection() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MemoryManager& TextureCollection::getMemoryManager() noexcept { return *memoryManager; }

    const MemoryManager& TextureCollection::getMemoryManager() const noexcept { return *memoryManager; }

    ////////////////////////////////////////////////////////////////
    // Images.
    ////////////////////////////////////////////////////////////////

    Image2D2& TextureCollection::createImage2D(const std::array<uint32_t, 2>& size,
                                               const VkFormat                 format,
                                               uint32_t                       levels,
                                               const VkImageUsageFlags        usage,
                                               const VkImageAspectFlags       aspect,
                                               const VkImageLayout            initialLayout,
                                               const VulkanQueueFamily&       initialOwner,
                                               const VkImageTiling            tiling)
    {
        assert(size[0] > 0 && size[1] > 0);

        // Calculate mips automatically.
        if (levels == 0) levels = static_cast<uint32_t>(std::floor(std::log2(std::max(size[0], size[1])))) + 1;

        VulkanImage::Settings imageSettings;
        imageSettings.device             = getMemoryManager().getDevice();
        imageSettings.format             = format;
        imageSettings.width              = size[0];
        imageSettings.height             = size[1];
        imageSettings.depth              = 1;
        imageSettings.mipLevels          = levels;
        imageSettings.arrayLayers        = 1;
        imageSettings.tiling             = tiling;
        imageSettings.imageUsage         = usage;
        imageSettings.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
        imageSettings.initialLayout      = initialLayout;
        imageSettings.allocator          = getMemoryManager().getAllocator();
        imageSettings.vma.memoryUsage    = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        imageSettings.vma.requiredFlags  = 0;
        imageSettings.vma.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        imageSettings.vma.flags          = 0;
        auto vulkanImage                 = VulkanImage::create(imageSettings);

        auto image = std::make_unique<Image2D2>(*this, uuids::uuid_system_generator{}());
        image->queueFamily.resize(levels, &initialOwner);
        image->imageLayout.resize(levels, initialLayout);
        image->image       = std::move(vulkanImage);
        image->format      = format;
        image->size        = size;
        image->usageFlags  = usage;
        image->aspectFlags = aspect;
        image->tiling      = tiling;

        return *images2D.emplace(image->getUuid(), std::move(image)).first->second;
    }

    Image2D2& TextureCollection::createImage2D(const Image2D2& image)
    {
        return createImage2D({image.getWidth(), image.getHeight()},
                             image.getFormat(),
                             image.getLevelCount(),
                             image.getImageUsageFlags(),
                             image.getImageAspectFlags(),
                             image.getImageLayout(0, 0),
                             image.getQueueFamily(0, 0),
                             image.getImageTiling());
    }

    void TextureCollection::destroyImage(const Image2D2& image)
    {
        // TODO: Do something with already staged transfers.
        // // TODO: What about textures referencing this image.
        if (&image.getTextureCollection() != this)
            throw SolError("Cannot destroy image that is part of a different texture collection.");
        const auto it = images2D.find(image.getUuid());
        if (it == images2D.end()) throw SolError("Cannot destroy image: image was already destroyed.");
        images2D.erase(it);
    }

    ////////////////////////////////////////////////////////////////
    // Samplers.
    ////////////////////////////////////////////////////////////////

    Sampler2D& TextureCollection::createSampler2D(const VkFilter             magFilter,
                                                  const VkFilter             minFilter,
                                                  const VkSamplerMipmapMode  mipmapMode,
                                                  const VkSamplerAddressMode addressModeU,
                                                  const VkSamplerAddressMode addressModeV,
                                                  const VkSamplerAddressMode addressModeW)
    {
        VulkanSampler::Settings samplerSettings;
        samplerSettings.device       = getMemoryManager().getDevice();
        samplerSettings.magFilter    = magFilter;
        samplerSettings.minFilter    = minFilter;
        samplerSettings.mipmapMode   = mipmapMode;
        samplerSettings.addressModeU = addressModeU;
        samplerSettings.addressModeV = addressModeV;
        samplerSettings.addressModeW = addressModeW;
        auto vulkanSampler           = VulkanSampler::create(samplerSettings);

        auto sampler = std::make_unique<Sampler2D>(*this, uuids::uuid_system_generator{}(), std::move(vulkanSampler));

        return *samplers2D.emplace(sampler->getUuid(), std::move(sampler)).first->second;
    }

    void TextureCollection::destroySampler(const Sampler2D& sampler)
    {
        if (&sampler.getTextureCollection() != this)
            throw SolError("Cannot destroy sampler that is part of a different texture collection.");
        const auto it = samplers2D.find(sampler.getUuid());
        if (it == samplers2D.end()) throw SolError("Cannot destroy sampler: sampler was already destroyed.");
        samplers2D.erase(it);
    }

    ////////////////////////////////////////////////////////////////
    // Textures.
    ////////////////////////////////////////////////////////////////

    Texture2D2& TextureCollection::createTexture2D(Image2D2& image, Sampler2D& sampler)
    {
        if (&image.getTextureCollection() != this)
            throw SolError("Cannot create texture for image that is part of a different texture collection.");
        if (&sampler.getTextureCollection() != this)
            throw SolError("Cannot create texture for sampler that is part of a different texture collection.");

        VulkanImageView::Settings viewSettings;
        viewSettings.image  = image.getImage();
        viewSettings.format = image.getFormat();
        viewSettings.aspect = image.getImageAspectFlags();
        auto vulkanView     = VulkanImageView::create(viewSettings);

        auto texture =
          std::make_unique<Texture2D2>(*this, uuids::uuid_system_generator{}(), image, sampler, std::move(vulkanView));

        return *textures2D.emplace(texture->getUuid(), std::move(texture)).first->second;
    }

    void TextureCollection::destroyTexture(const Texture2D2& texture)
    {
        if (&texture.getTextureCollection() != this)
            throw SolError("Cannot destroy texture that is part of a different texture collection.");
        const auto it = textures2D.find(texture.getUuid());
        if (it == textures2D.end()) throw SolError("Cannot destroy texture: texture was already destroyed.");
        textures2D.erase(it);
    }
}  // namespace sol
