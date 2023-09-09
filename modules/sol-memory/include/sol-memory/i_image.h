#pragma once
////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/fwd.h"

namespace sol
{
    class IImage
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class ImageType : uint32_t
        {
            Image1D = 1,
            Image2D = 2,
            Image3D = 3,
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IImage() = delete;

        IImage(MemoryManager& memoryManager, uuids::uuid id);

        IImage(const IImage&) = delete;

        IImage(IImage&&) noexcept = default;

        virtual ~IImage() noexcept;

        IImage& operator=(const IImage&) = delete;

        IImage& operator=(IImage&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        /**
         * \brief Get the queue family that currently owns this resource.
         * \param level Mip level to get owner for.
         * \param layer Array layer to get owner for.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] virtual const VulkanQueueFamily& getQueueFamily(uint32_t level, uint32_t layer) const = 0;

        /**
         * \brief Get the underlying vulkan image object.
         * \return VulkanImage.
         */
        [[nodiscard]] virtual VulkanImage& getImage() = 0;

        /**
         * \brief Get the underlying vulkan image object.
         * \return VulkanImage.
         */
        [[nodiscard]] virtual const VulkanImage& getImage() const = 0;

        /**
         * \brief Get the image type.
         * \return ImageType.
         */
        [[nodiscard]] virtual ImageType getImageType() const noexcept = 0;

        /**
         * \brief Get the number of mip levels.
         * \return Level count.
         */
        [[nodiscard]] virtual uint32_t getLevelCount() const noexcept = 0;

        /**
         * \brief Get the number of array layers.
         * \return Layer count.
         */
        [[nodiscard]] virtual uint32_t getLayerCount() const noexcept = 0;

        /**
         * \brief Get the size of the image in pixels.
         * \return Size.
         */
        [[nodiscard]] virtual std::array<uint32_t, 3> getSize() const noexcept = 0;

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
         * \brief Get the image depth in pixels.
         * \return Depth.
         */
        [[nodiscard]] uint32_t getDepth() const noexcept;

        [[nodiscard]] virtual VkFormat getFormat() const noexcept = 0;

        [[nodiscard]] virtual VkImageUsageFlags getImageUsageFlags() const noexcept = 0;

        [[nodiscard]] virtual VkImageAspectFlags getImageAspectFlags() const noexcept = 0;

        [[nodiscard]] virtual VkImageLayout getImageLayout(uint32_t level, uint32_t layer) const = 0;

        [[nodiscard]] virtual VkImageTiling getImageTiling() const = 0;

        [[nodiscard]] VkSubresourceLayout getSubresourceLayout(uint32_t level, uint32_t layer) const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the queue family that owns the specified level and layer of this image. 
         * \param family New queue family.
         * \param level Mip level to set owner for. If -1, set for all levels.
         * \param layer Array layer to set owner for. If -1, set for all layers.
         */
        virtual void setQueueFamily(const VulkanQueueFamily& family, uint32_t level, uint32_t layer) = 0;

        /**
         * \brief Set the layout of the image for the the specified level and layer of this image.
         * \param layout Layout.
         * \param level Mip level to set layout for. If -1, set for all levels.
         * \param layer Array layer to set layout for. If -1, set for all layers.
         */
        virtual void setImageLayout(VkImageLayout layout, uint32_t level, uint32_t layer) = 0;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* manager = nullptr;

        uuids::uuid uuid;
    };
}  // namespace sol
