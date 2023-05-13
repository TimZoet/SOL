#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

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
#include "sol-texture/image_transfer/image_staging_buffer.h"

namespace sol
{
    class TextureManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TextureManager() = default;

        explicit TextureManager(MemoryManager& memManager);

        TextureManager(MemoryManager& memManager, IImageTransferPtr imgTransfer);

        TextureManager(const TextureManager&) = delete;

        TextureManager(TextureManager&&) = delete;

        ~TextureManager() noexcept;

        TextureManager& operator=(const TextureManager&) = delete;

        TextureManager& operator=(TextureManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the MemoryManager.
         * \return MemoryManager.
         */
        MemoryManager& getMemoryManager() noexcept;

        /**
         * \brief Get the MemoryManager.
         * \return MemoryManager.
         */
        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Images.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new 2D image with uninitialized data.
         * \param format Image format.
         * \param size Image size in pixels.
         * \return Image.
         */
        Image2D& createImage2D(VkFormat format, const std::array<uint32_t, 2>& size);

        /**
         * \brief Create a new 2D image with uninitialized data.
         * \param format Image format.
         * \param size Image size in pixels.
         * \param usage Image usage flags.
         * \return Image.
         */
        Image2D& createImage2D(VkFormat format, const std::array<uint32_t, 2>& size, VkImageUsageFlags usage);

        /**
         * \brief Create a new 2D image and fill it with all default values. Automatically creates and fills a staging buffer.
         * \param format Image format.
         * \param size Image size in pixels.
         * \param value Default value for each pixel. Size should be equal to the size of a single pixel value.
         * \return Image.
         */
        Image2D& createImage2DFill(VkFormat format, const std::array<uint32_t, 2>& size, const void* value);

        /**
         * \brief Create a new 2D image and fill it with all default values. Automatically creates and fills a staging buffer.
         * \param format Image format.
         * \param size Image size in pixels.
         * \param value Default value for each pixel. Size should be equal to the size of a single pixel value.
         * \param usage Image usage flags.
         * \return Image.
         */
        Image2D& createImage2DFill(VkFormat                       format,
                                   const std::array<uint32_t, 2>& size,
                                   const void*                    value,
                                   VkImageUsageFlags              usage);

        /**
         * \brief Create a new 2D image and directly fill it with the given data array. Automatically creates and fills a staging buffer.
         * \param format Image format.
         * \param size Image size in pixels.
         * \param data Data array. Should contain at least as much data as the size of the image buffer.
         * \return Image.
         */
        Image2D& createImage2D(VkFormat format, const std::array<uint32_t, 2>& size, const void* data);

        /**
         * \brief Create a new 2D image and directly fill it with the given data array. Automatically creates and fills a staging buffer.
         * \param format Image format.
         * \param size Image size in pixels.
         * \param data Data array. Should contain at least as much data as the size of the image buffer.
         * \param usage Image usage flags.
         * \return Image.
         */
        Image2D& createImage2D(VkFormat                       format,
                               const std::array<uint32_t, 2>& size,
                               const void*                    data,
                               VkImageUsageFlags              usage);

        void destroyImage2D(Image2D& image);

        ////////////////////////////////////////////////////////////////
        // Textures.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new 2D texture for a 2D image. Constructs a default image view and sampler.
         * \param image2D Image.
         * \return Texture.
         */
        Texture2D& createTexture2D(Image2D& image2D);

        /**
         * \brief Create a new 2D texture for a 2D image. Allows passing in a custom image view and/or sampler.
         * \param image2D Image.
         * \param imageView If not null, the image view to use in place of the default.
         * \param sampler If not null, the sampler to use in place of the default.
         * \return Texture.
         */
        Texture2D& createTexture2D(Image2D& image2D, VulkanImageViewPtr imageView, VulkanSamplerPtr sampler);

        void destroyTexture2D(Texture2D& texture);

        ////////////////////////////////////////////////////////////////
        // Transfer.
        ////////////////////////////////////////////////////////////////

        size_t createStagingBuffer(Image2D&                       image,
                                   const std::array<uint32_t, 2>& regionOffset,
                                   const std::array<uint32_t, 2>& regionSize) const;

        [[nodiscard]] ImageStagingBuffer getStagingBuffer(Image2D& image, size_t index) const;

        void stageLayoutTransition(Image2D& image) const;

        void stageOwnershipTransfer(Image2D& image) const;

        void transfer() const;

    private:
        Image2D& createImage2DImpl(VkFormat format, const std::array<uint32_t, 2>& size, VkImageUsageFlags usage);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        std::vector<Image2DPtr> images2D;

        std::vector<Texture2DPtr> textures2D;

        IImageTransferPtr imageTransfer;
    };
}  // namespace sol
