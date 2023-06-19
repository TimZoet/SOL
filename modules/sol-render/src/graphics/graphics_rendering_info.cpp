#include "sol-render/graphics/graphics_rendering_info.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_image_view.h"
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
    // Getters
    ////////////////////////////////////////////////////////////////

    bool GraphicsRenderingInfo::isFinalized() const noexcept
    {
        return renderingInfo.sType == VK_STRUCTURE_TYPE_RENDERING_INFO;
    }

    void GraphicsRenderingInfo::requireFinalized() const
    {
        if (!isFinalized()) throw SolError("GraphicsRenderingInfo was not yet finalized.");
    }

    void GraphicsRenderingInfo::requireNonFinalized() const
    {
        if (isFinalized()) throw SolError("GraphicsRenderingInfo was already finalized.");
    }

    const VkRenderingInfo& GraphicsRenderingInfo::get() const
    {
        requireFinalized();
        return renderingInfo;
    }

    const std::vector<const VulkanImageView*>& GraphicsRenderingInfo::getColorImageViews() const noexcept
    {
        return colorImageViews;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderingInfo::setRenderArea(const VkRect2D area)
    {
        requireNonFinalized();
        renderArea = area;
    }

    void GraphicsRenderingInfo::setRenderArea(const VkOffset2D offset, const VkExtent2D extent)
    {
        requireNonFinalized();
        renderArea.offset = offset;
        renderArea.extent = extent;
    }

    void GraphicsRenderingInfo::setRenderArea(const int32_t  offsetX,
                                              const int32_t  offsetY,
                                              const uint32_t extentX,
                                              const uint32_t extentY)
    {
        requireNonFinalized();
        renderArea.offset.x      = offsetX;
        renderArea.offset.y      = offsetY;
        renderArea.extent.width  = extentX;
        renderArea.extent.height = extentY;
    }

    void GraphicsRenderingInfo::setLayerCount(const uint32_t count)
    {
        requireNonFinalized();
        layerCount = count;
    }

    void GraphicsRenderingInfo::addColorAttachment(const VulkanImageView&     imageView,
                                                   const VkImageLayout        imageLayout,
                                                   const VkAttachmentLoadOp   loadOp,
                                                   const VkAttachmentStoreOp  storeOp,
                                                   const std::array<float, 4> clearColor)
    {
        requireNonFinalized();
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
        requireNonFinalized();
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
        requireNonFinalized();
        VkClearColorValue clear;
        std::ranges::copy(clearColor, clear.uint32);
        addColorAttachment(imageView, imageLayout, loadOp, storeOp, clear);
    }

    void GraphicsRenderingInfo::addDepthAttachment(const VulkanImageView&    imageView,
                                                   const VkImageLayout       imageLayout,
                                                   const VkAttachmentLoadOp  loadOp,
                                                   const VkAttachmentStoreOp storeOp,
                                                   const float               clearDepth)
    {
        requireNonFinalized();
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
        requireNonFinalized();
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
        requireNonFinalized();

        colorImageViews.emplace_back(&imageView);

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

    void GraphicsRenderingInfo::finalize()
    {
        requireNonFinalized();
        renderingInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.pNext                = nullptr;
        renderingInfo.flags                = 0;
        renderingInfo.renderArea           = renderArea;
        renderingInfo.layerCount           = layerCount;
        renderingInfo.viewMask             = 0;
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentInfo.size());
        renderingInfo.pColorAttachments    = colorAttachmentInfo.data();
        renderingInfo.pDepthAttachment     = depthAttachmentInfo ? &depthAttachmentInfo.value() : VK_NULL_HANDLE;
        renderingInfo.pStencilAttachment   = stencilAttachmentInfo ? &stencilAttachmentInfo.value() : VK_NULL_HANDLE;
    }
}  // namespace sol
