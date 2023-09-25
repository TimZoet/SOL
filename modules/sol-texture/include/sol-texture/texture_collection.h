#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

namespace sol
{
    class TextureCollection
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TextureCollection() = delete;

        explicit TextureCollection(MemoryManager& manager);

        TextureCollection(const TextureCollection&) = delete;

        TextureCollection(TextureCollection&&) noexcept = delete;

        ~TextureCollection() noexcept;

        TextureCollection& operator=(const TextureCollection&) = delete;

        TextureCollection& operator=(TextureCollection&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the memory manager.
         * \return MemoryManager.
         */
        MemoryManager& getMemoryManager() noexcept;

        /**
         * \brief Get the memory manager.
         * \return MemoryManager.
         */
        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Images.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new 2D image with uninitialized data.
         * \param size Image size in pixels.
         * \param format Image format.
         * \param levels Number of mip levels.
         * If == 0, the number of mips is calculated automatically as log_2 of the size.
         * If == 1, no mips are used. If > 1, the value is used explicitly.
         * \param usage Image usage.
         * \param aspect Image aspect.
         * \param initialLayout Initial image layout (can be VK_IMAGE_LAYOUT_UNDEFINED).
         * \param initialOwner Initial queue family that owns the image.
         * \param tiling Image tiling.
         * \return Image.
         */
        Image2D2& createImage2D(const std::array<uint32_t, 2>& size,
                                VkFormat                       format,
                                uint32_t                       levels,
                                VkImageUsageFlags              usage,
                                VkImageAspectFlags             aspect,
                                VkImageLayout                  initialLayout,
                                const VulkanQueueFamily&       initialOwner,
                                VkImageTiling                  tiling);

        void destroyImage(Image2D2& image);

        ////////////////////////////////////////////////////////////////
        // Samplers.
        ////////////////////////////////////////////////////////////////

        Sampler2D& createSampler2D(VkFilter             magFilter    = VK_FILTER_LINEAR,
                                   VkFilter             minFilter    = VK_FILTER_LINEAR,
                                   VkSamplerMipmapMode  mipmapMode   = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                   VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                   VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                   VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

        ////////////////////////////////////////////////////////////////
        // Textures.
        ////////////////////////////////////////////////////////////////

        Texture2D2& createTexture2D(Image2D2& image, Sampler2D& sampler);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        std::unordered_map<uuids::uuid, Image2D2Ptr> images2D;

        std::unordered_map<uuids::uuid, Sampler2DPtr> samplers2D;

        std::unordered_map<uuids::uuid, Texture2D2Ptr> textures2D;
    };
}  // namespace sol
