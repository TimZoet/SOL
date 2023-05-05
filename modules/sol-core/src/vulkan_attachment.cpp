#include "sol-core/vulkan_attachment.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_render_pass_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanAttachment::VulkanAttachment(VulkanRenderPassLayout& rpLayout, const uint32_t attachmentIndex) :
        layout(&rpLayout), index(attachmentIndex)
    {
    }

    VulkanAttachment::VulkanAttachment(const VulkanAttachment& other) { *this = other; }

    VulkanAttachment::~VulkanAttachment() noexcept = default;

    VulkanAttachment& VulkanAttachment::operator=(const VulkanAttachment& other)
    {
        flags          = other.flags;
        format         = other.format;
        samples        = other.samples;
        loadOp         = other.loadOp;
        storeOp        = other.storeOp;
        stencilLoadOp  = other.stencilLoadOp;
        stencilStoreOp = other.stencilStoreOp;
        initialLayout  = other.initialLayout;
        finalLayout    = other.finalLayout;
        return *this;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanRenderPassLayout& VulkanAttachment::getLayout() noexcept { return *layout; }

    const VulkanRenderPassLayout& VulkanAttachment::getLayout() const noexcept { return *layout; }

    uint32_t VulkanAttachment::getIndex() const noexcept { return index; }

    VkAttachmentDescriptionFlags VulkanAttachment::getDescriptionFlags() const noexcept { return flags; }

    VkFormat VulkanAttachment::getFormat() const noexcept { return format; }

    VkSampleCountFlagBits VulkanAttachment::getSamples() const noexcept { return samples; }

    VkAttachmentLoadOp VulkanAttachment::getLoadOp() const noexcept { return loadOp; }

    VkAttachmentStoreOp VulkanAttachment::getStoreOp() const noexcept { return storeOp; }

    VkAttachmentLoadOp VulkanAttachment::getStencilLoadOp() const noexcept { return stencilLoadOp; }

    VkAttachmentStoreOp VulkanAttachment::getStencilStoreOp() const noexcept { return stencilStoreOp; }

    VkImageLayout VulkanAttachment::getInitialLayout() const noexcept { return initialLayout; }

    VkImageLayout VulkanAttachment::getFinalLayout() const noexcept { return finalLayout; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void VulkanAttachment::setDescriptionFlags(const VkAttachmentDescriptionFlags descriptionFlags)
    {
        layout->requireNonFinalized();
        flags = descriptionFlags;
    }

    void VulkanAttachment::setFormat(const VkFormat f)
    {
        layout->requireNonFinalized();
        format = f;
    }

    void VulkanAttachment::setSamples(const VkSampleCountFlagBits s)
    {
        layout->requireNonFinalized();
        samples = s;
    }

    void VulkanAttachment::setLoadOp(const VkAttachmentLoadOp op)
    {
        layout->requireNonFinalized();
        loadOp = op;
    }

    void VulkanAttachment::setStoreOp(const VkAttachmentStoreOp op)
    {
        layout->requireNonFinalized();
        storeOp = op;
    }

    void VulkanAttachment::setStencilLoadOp(const VkAttachmentLoadOp op)
    {
        layout->requireNonFinalized();
        stencilLoadOp = op;
    }

    void VulkanAttachment::setStencilStoreOp(const VkAttachmentStoreOp op)
    {
        layout->requireNonFinalized();
        stencilStoreOp = op;
    }

    void VulkanAttachment::setInitialLayout(const VkImageLayout imageLayout)
    {
        layout->requireNonFinalized();
        initialLayout = imageLayout;
    }

    void VulkanAttachment::setFinalLayout(const VkImageLayout imageLayout)
    {
        layout->requireNonFinalized();
        finalLayout = imageLayout;
    }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void VulkanAttachment::destroy() const
    {
        layout->requireNonFinalized();
        layout->destroyAttachment(index);
    }
}  // namespace sol
