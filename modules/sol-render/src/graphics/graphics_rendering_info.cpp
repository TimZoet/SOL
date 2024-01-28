#include "sol-render/graphics/graphics_rendering_info.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-error/sol_error.h"
#include "sol-material/graphics/graphics_material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderingInfo::GraphicsRenderingInfo() = default;

    GraphicsRenderingInfo::~GraphicsRenderingInfo() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderingInfo::setRenderArea(const VkRect2D area) { renderArea = area; }

    void GraphicsRenderingInfo::setRenderArea(const VkOffset2D offset, const VkExtent2D extent)
    {
        renderArea.offset = offset;
        renderArea.extent = extent;
    }

    void GraphicsRenderingInfo::setRenderArea(const int32_t  offsetX,
                                              const int32_t  offsetY,
                                              const uint32_t extentX,
                                              const uint32_t extentY)
    {
        renderArea.offset.x      = offsetX;
        renderArea.offset.y      = offsetY;
        renderArea.extent.width  = extentX;
        renderArea.extent.height = extentY;
    }

    void GraphicsRenderingInfo::setLayerCount(const uint32_t count) { layerCount = count; }

    void GraphicsRenderingInfo::addColorAttachment(const VulkanImageView&     imageView,
                                                   const VkImageLayout        imageLayout,
                                                   const VkAttachmentLoadOp   loadOp,
                                                   const VkAttachmentStoreOp  storeOp,
                                                   const std::array<float, 4> clearColor)
    {
        VkClearColorValue clear;
        std::ranges::copy(clearColor, clear.float32);
        addColorAttachment(imageView, imageLayout, loadOp, storeOp, clear);
    }

    void GraphicsRenderingInfo::addColorAttachment(const VulkanImageView&       imageView,
                                                   const VkImageLayout          imageLayout,
                                                   const VkAttachmentLoadOp     loadOp,
                                                   const VkAttachmentStoreOp    storeOp,
                                                   const std::array<int32_t, 4> clearColor)
    {
        VkClearColorValue clear;
        std::ranges::copy(clearColor, clear.int32);
        addColorAttachment(imageView, imageLayout, loadOp, storeOp, clear);
    }

    void GraphicsRenderingInfo::addColorAttachment(const VulkanImageView&        imageView,
                                                   const VkImageLayout           imageLayout,
                                                   const VkAttachmentLoadOp      loadOp,
                                                   const VkAttachmentStoreOp     storeOp,
                                                   const std::array<uint32_t, 4> clearColor)
    {
        VkClearColorValue clear;
        std::ranges::copy(clearColor, clear.uint32);
        addColorAttachment(imageView, imageLayout, loadOp, storeOp, clear);
    }

    void GraphicsRenderingInfo::setColorAttachmentPreTransition(const size_t                index,
                                                                const VulkanQueueFamily*    srcQueue,
                                                                const VulkanQueueFamily*    dstQueue,
                                                                const VkImageLayout         oldLayout,
                                                                const VkImageLayout         newLayout,
                                                                const VkPipelineStageFlags2 srcStage,
                                                                const VkPipelineStageFlags2 dstStage,
                                                                const VkAccessFlags2        srcAccess,
                                                                const VkAccessFlags2        dstAccess)
    {
        preColorTransitions.at(index) =
          ColorAttachmentTransition{srcQueue, dstQueue, oldLayout, newLayout, srcStage, dstStage, srcAccess, dstAccess};
    }

    void GraphicsRenderingInfo::setColorAttachmentPostTransition(const size_t                index,
                                                                 const VulkanQueueFamily*    srcQueue,
                                                                 const VulkanQueueFamily*    dstQueue,
                                                                 const VkImageLayout         oldLayout,
                                                                 const VkImageLayout         newLayout,
                                                                 const VkPipelineStageFlags2 srcStage,
                                                                 const VkPipelineStageFlags2 dstStage,
                                                                 const VkAccessFlags2        srcAccess,
                                                                 const VkAccessFlags2        dstAccess)
    {
        postColorTransitions.at(index) =
          ColorAttachmentTransition{srcQueue, dstQueue, oldLayout, newLayout, srcStage, dstStage, srcAccess, dstAccess};
    }

    void GraphicsRenderingInfo::addDepthAttachment(const VulkanImageView&    imageView,
                                                   const VkImageLayout       imageLayout,
                                                   const VkAttachmentLoadOp  loadOp,
                                                   const VkAttachmentStoreOp storeOp,
                                                   const float               clearDepth)
    {
        VkClearValue clear;
        clear.depthStencil.depth = clearDepth;
        colorAttachmentInfo.emplace_back(VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                                         nullptr,
                                         imageView.get(),
                                         imageLayout,
                                         VK_RESOLVE_MODE_NONE,
                                         VK_NULL_HANDLE,
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         loadOp,
                                         storeOp,
                                         clear);
    }

    void GraphicsRenderingInfo::addStencilAttachment(const VulkanImageView&    imageView,
                                                     const VkImageLayout       imageLayout,
                                                     const VkAttachmentLoadOp  loadOp,
                                                     const VkAttachmentStoreOp storeOp,
                                                     const uint32_t            clearStencil)
    {
        VkClearValue clear;
        clear.depthStencil.stencil = clearStencil;
        colorAttachmentInfo.emplace_back(VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                                         nullptr,
                                         imageView.get(),
                                         imageLayout,
                                         VK_RESOLVE_MODE_NONE,
                                         VK_NULL_HANDLE,
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         loadOp,
                                         storeOp,
                                         clear);
    }

    void GraphicsRenderingInfo::addColorAttachment(const VulkanImageView&    imageView,
                                                   const VkImageLayout       imageLayout,
                                                   const VkAttachmentLoadOp  loadOp,
                                                   const VkAttachmentStoreOp storeOp,
                                                   const VkClearColorValue   clearColor)
    {
        colorImageViews.emplace_back(&imageView);
        preColorTransitions.emplace_back();
        postColorTransitions.emplace_back();

        VkClearValue clear;
        clear.color = clearColor;
        colorAttachmentInfo.emplace_back(VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                                         nullptr,
                                         imageView.get(),
                                         imageLayout,
                                         VK_RESOLVE_MODE_NONE,
                                         VK_NULL_HANDLE,
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         loadOp,
                                         storeOp,
                                         clear);
    }

    void GraphicsRenderingInfo::beginRendering(const VulkanCommandBuffer& buffer) const
    {
        transition(buffer, preColorTransitions);

        const VkRenderingInfo renderingInfo{
          .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
          .pNext                = nullptr,
          .flags                = 0,
          .renderArea           = renderArea,
          .layerCount           = layerCount,
          .viewMask             = 0,
          .colorAttachmentCount = static_cast<uint32_t>(colorAttachmentInfo.size()),
          .pColorAttachments    = colorAttachmentInfo.data(),
          .pDepthAttachment     = depthAttachmentInfo ? &depthAttachmentInfo.value() : VK_NULL_HANDLE,
          .pStencilAttachment   = stencilAttachmentInfo ? &stencilAttachmentInfo.value() : VK_NULL_HANDLE};

        vkCmdBeginRendering(buffer.get(), &renderingInfo);
    }

    void GraphicsRenderingInfo::endRendering(const VulkanCommandBuffer& buffer) const
    {
        vkCmdEndRendering(buffer.get());
        transition(buffer, postColorTransitions);
    }

    void
      GraphicsRenderingInfo::transition(const VulkanCommandBuffer&                                   buffer,
                                        const std::vector<std::optional<ColorAttachmentTransition>>& transitions) const
    {
        std::vector<VkImageMemoryBarrier2> barriers;

        for (const auto [image, transition] :
             std::views::zip(colorImageViews, transitions) |
               std::views::filter([](const auto& pair) -> bool { return std::get<1>(pair).has_value(); }))
        {
            barriers.emplace_back(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                  nullptr,
                                  transition->srcStage,
                                  transition->srcAccess,
                                  transition->dstStage,
                                  transition->dstAccess,
                                  transition->oldLayout,
                                  transition->newLayout,
                                  transition->srcQueue ? transition->srcQueue->getIndex() : VK_QUEUE_FAMILY_IGNORED,
                                  transition->dstQueue ? transition->dstQueue->getIndex() : VK_QUEUE_FAMILY_IGNORED,
                                  image->getImage().get(),
                                  VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }

        if (!barriers.empty())
        {
            const VkDependencyInfo info{.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                        .imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
                                        .pImageMemoryBarriers    = barriers.data()};
            vkCmdPipelineBarrier2(buffer.get(), &info);
        }
    }
}  // namespace sol
