#include "sol-texture/image2d.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image.h"
#include "sol-error/sol_error.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/texture_manager.h"

namespace
{
    void checkRegion(const std::array<uint32_t, 2>& size,
                     const std::array<uint32_t, 2>& regionOffset,
                     const std::array<uint32_t, 2>& regionSize)
    {
        if (regionOffset[0] + regionSize[0] > size[0] || regionOffset[1] + regionSize[1] > size[1])
            throw sol::SolError("Region is not within the bounds of the image.");
    }
}  // namespace

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Image2D::Image2D(TextureManager&               manager,
                     const VkFormat                vkFormat,
                     const std::array<uint32_t, 2> imgSize,
                     const VkImageUsageFlags       vkUsage) :
        textureManager(&manager), format(vkFormat), size(imgSize), usage(vkUsage)
    {
        initialize();
    }

    Image2D::~Image2D() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    TextureManager& Image2D::getTextureManager() noexcept { return *textureManager; }

    const TextureManager& Image2D::getTextureManager() const noexcept { return *textureManager; }

    VulkanImage& Image2D::getImage() noexcept { return *image; }

    const VulkanImage& Image2D::getImage() const noexcept { return *image; }

    VkFormat Image2D::getFormat() const noexcept { return format; }

    std::array<uint32_t, 2> Image2D::getSize() const noexcept { return size; }

    uint32_t Image2D::getWidth() const noexcept { return size[0]; }

    uint32_t Image2D::getHeight() const noexcept { return size[1]; }

    VkImageUsageFlags Image2D::getImageUsageFlags() const noexcept { return usage; }

    const VulkanQueueFamily* Image2D::getCurrentFamily() const noexcept { return currentFamily; }

    const VulkanQueueFamily* Image2D::getTargetFamily() const noexcept { return targetFamily; }

    VkImageLayout Image2D::getCurrentLayout() const noexcept { return currentLayout; }

    VkImageLayout Image2D::getTargetLayout() const noexcept { return targetLayout; }

    VkPipelineStageFlags Image2D::getStageFlags() const noexcept { return stageFlags; }

    VkAccessFlags Image2D::getAccessFlags() const noexcept { return accessFlags; }

    VkImageAspectFlags Image2D::getAspectFlags() const noexcept { return aspectFlags; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Image2D::setCurrentFamily(const VulkanQueueFamily& family) { currentFamily = &family; }

    void Image2D::setTargetFamily(const VulkanQueueFamily& family)
    {
        if (&family != targetFamily)
        {
            targetFamily = &family;
            if (targetFamily != currentFamily) textureManager->stageOwnershipTransfer(*this);
        }
    }

    void Image2D::setCurrentLayout(const VkImageLayout layout) { currentLayout = layout; }

    void Image2D::setTargetLayout(const VkImageLayout layout)
    {
        if (layout != targetLayout)
        {
            targetLayout = layout;
            if (layout != currentLayout) textureManager->stageLayoutTransition(*this);
        }
    }

    void Image2D::setStageFlags(const VkPipelineStageFlags flags) { stageFlags = flags; }

    void Image2D::setAccessFlags(const VkAccessFlags flags) { accessFlags = flags; }

    void Image2D::setAspectFlags(const VkImageAspectFlags flags) { aspectFlags = flags; }

    ////////////////////////////////////////////////////////////////
    // Data setters.
    ////////////////////////////////////////////////////////////////

    size_t Image2D::createStagingBuffer() { return createStagingBuffer({0, 0}, size); }

    size_t Image2D::createStagingBuffer(const std::array<uint32_t, 2>& regionOffset,
                                        const std::array<uint32_t, 2>& regionSize)
    {
        checkRegion(size, regionOffset, regionSize);
        return textureManager->createStagingBuffer(*this, regionOffset, regionSize);
    }

    void Image2D::setData(const void* data, const size_t stagingBufferIndex)
    {
        return setData(data, {0, 0}, size, stagingBufferIndex);
    }

    void Image2D::setData(const void*                    data,
                          const std::array<uint32_t, 2>& regionOffset,
                          const std::array<uint32_t, 2>& regionSize,
                          const size_t                   stagingBufferIndex)
    {
        checkRegion(size, regionOffset, regionSize);

        const auto stagingBuffer = textureManager->getStagingBuffer(*this, stagingBufferIndex);
        stagingBuffer.getStagingBuffer().setData2D(
          data, stagingBuffer.getSize(), stagingBuffer.toLocal(regionOffset), regionSize);
    }

    void Image2D::fill(const void* value, const size_t stagingBufferIndex)
    {
        return fill(value, {0, 0}, size, stagingBufferIndex);
    }

    void Image2D::fill(const void*                    value,
                       const std::array<uint32_t, 2>& regionOffset,
                       const std::array<uint32_t, 2>& regionSize,
                       const size_t                   stagingBufferIndex)
    {
        checkRegion(size, regionOffset, regionSize);

        const auto stagingBuffer = textureManager->getStagingBuffer(*this, stagingBufferIndex);
        stagingBuffer.getStagingBuffer().setDataRepeated2D(
          value, stagingBuffer.getSize(), stagingBuffer.toLocal(regionOffset), regionSize);
    }

    ////////////////////////////////////////////////////////////////
    // Utility methods.
    ////////////////////////////////////////////////////////////////

    void Image2D::initialize()
    {
        aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

        VulkanImage::Settings imageSettings;
        imageSettings.device         = textureManager->getMemoryManager().getDevice();
        imageSettings.format         = format;
        imageSettings.width          = size[0];
        imageSettings.height         = size[1];
        imageSettings.depth          = 1;
        imageSettings.tiling         = VK_IMAGE_TILING_OPTIMAL;
        imageSettings.imageUsage     = usage;
        imageSettings.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;
        imageSettings.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        imageSettings.allocator      = textureManager->getMemoryManager().getAllocator();
        imageSettings.memoryUsage    = VMA_MEMORY_USAGE_GPU_ONLY;
        imageSettings.requiredFlags  = 0;
        imageSettings.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        imageSettings.flags          = 0;
        image                        = VulkanImage::create(imageSettings);
    }
}  // namespace sol
