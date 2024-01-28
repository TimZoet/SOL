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

#include "sol-core/vulkan_queue_family.h"
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

    VkImageAspectFlags Image2D::getAspectFlags() const noexcept { return aspectFlags; }

    const VulkanQueueFamily* Image2D::getQueueFamily() const noexcept { return queueFamily; }

    VkImageLayout Image2D::getImageLayout() const noexcept { return imageLayout; }

    ////////////////////////////////////////////////////////////////
    // Transitions.
    ////////////////////////////////////////////////////////////////

    void Image2D::stageTransition(const VulkanQueueFamily*           family,
                                  const std::optional<VkImageLayout> layout,
                                  const VkPipelineStageFlags2        srcStage,
                                  const VkPipelineStageFlags2        dstStage,
                                  const VkAccessFlags2               srcAccess,
                                  const VkAccessFlags2               dstAccess)
    {
        if (!family && !layout)
            throw SolError("Cannot stage transition without either a family transfer or layout transition.");
        if (!queueFamily && !family) throw SolError("Cannot stage transition without source and target queue family.");

        stagedTransition = Transition{family, layout, srcStage, dstStage, srcAccess, dstAccess};
        textureManager->stageTransition(*this);
    }

    std::optional<Image2D::Transition> Image2D::getStagedTransition() const noexcept { return stagedTransition; }

    Image2D::BarrierPair Image2D::getTransitionBarriers() const
    {
        BarrierPair barriers;

        // If the image has an owner that is not the target queue, a barrier on both queues is needed.
        // Otherwise, only an acquire barrier on the target queue is needed.

        barriers.acquire = VkImageMemoryBarrier2{
          .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
          .pNext               = nullptr,
          .srcStageMask        = stagedTransition->srcStage,
          .srcAccessMask       = stagedTransition->srcAccess,
          .dstStageMask        = VK_PIPELINE_STAGE_2_NONE,
          .dstAccessMask       = VK_ACCESS_2_NONE,
          .oldLayout           = stagedTransition->newLayout ? imageLayout : VK_IMAGE_LAYOUT_UNDEFINED,
          .newLayout           = stagedTransition->newLayout ? *stagedTransition->newLayout : VK_IMAGE_LAYOUT_UNDEFINED,
          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
          .image               = image->get(),
          .subresourceRange    = {
               .aspectMask = aspectFlags, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

        if (queueFamily && queueFamily != stagedTransition->targetFamily)
        {
            barriers.acquire->srcQueueFamilyIndex = queueFamily->getIndex();
            barriers.acquire->dstQueueFamilyIndex = stagedTransition->targetFamily->getIndex();
            barriers.release                      = barriers.acquire;

            // We must only set the destination stage and access on the acquire barrier.
            // This to prevent e.g. a graphics only stage being recorded on a transfer queue, which is invalid.
            barriers.acquire->dstStageMask  = stagedTransition->dstStage;
            barriers.acquire->dstAccessMask = stagedTransition->dstAccess;
        }

        return barriers;
    }


    void Image2D::applyTransition()
    {
        if (!stagedTransition) return;

        if (stagedTransition->targetFamily) queueFamily = stagedTransition->targetFamily;
        if (stagedTransition->newLayout) imageLayout = *stagedTransition->newLayout;
        stagedTransition.reset();
    }

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
        imageSettings.device             = textureManager->getMemoryManager().getDevice();
        imageSettings.format             = format;
        imageSettings.width              = size[0];
        imageSettings.height             = size[1];
        imageSettings.depth              = 1;
        imageSettings.tiling             = VK_IMAGE_TILING_OPTIMAL;
        imageSettings.imageUsage         = usage;
        imageSettings.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
        imageSettings.initialLayout      = VK_IMAGE_LAYOUT_UNDEFINED;
        imageSettings.allocator          = textureManager->getMemoryManager().getAllocator();
        imageSettings.vma.memoryUsage    = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        imageSettings.vma.requiredFlags  = 0;
        imageSettings.vma.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        imageSettings.vma.flags          = 0;
        image                            = VulkanImage::create(imageSettings);
    }
}  // namespace sol
