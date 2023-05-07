#include "sol-core/vulkan_subpass.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_attachment.h"
#include "sol-core/vulkan_render_pass_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanSubpass::VulkanSubpass(VulkanRenderPassLayout& rpLayout, const uint32_t subpassIndex) :
        layout(&rpLayout), index(subpassIndex)
    {
    }

    VulkanSubpass::VulkanSubpass(const VulkanSubpass& other) { *this = other; }

    VulkanSubpass::~VulkanSubpass() noexcept = default;

    VulkanSubpass& VulkanSubpass::operator=(const VulkanSubpass& other)
    {
        // Always copy simple properties.
        flags             = other.flags;
        pipelineBindPoint = other.pipelineBindPoint;

        // Only copy attachments when in the same layout.
        if (layout == other.layout)
        {
            inputAttachments       = other.inputAttachments;
            colorAttachments       = other.colorAttachments;
            resolveAttachments     = other.resolveAttachments;
            depthStencilAttachment = other.depthStencilAttachment;
            resolveAttachments     = other.resolveAttachments;
        }

        return *this;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanRenderPassLayout& VulkanSubpass::getLayout() noexcept { return *layout; }

    const VulkanRenderPassLayout& VulkanSubpass::getLayout() const noexcept { return *layout; }

    uint32_t VulkanSubpass::getIndex() const noexcept { return index; }

    VkSubpassDescriptionFlags VulkanSubpass::getDescriptionFlags() const noexcept { return flags; }

    VkPipelineBindPoint VulkanSubpass::getPipelineBindPoint() const noexcept { return pipelineBindPoint; }

    size_t VulkanSubpass::getTotalAttachmentCount() const noexcept
    {
        size_t count = 0;
        count += inputAttachments.size();
        count += colorAttachments.size();
        count += resolveAttachments.size();
        count += depthStencilAttachment ? 1 : 0;
        return count;
    }

    const std::vector<VulkanSubpass::AttachmentReference>& VulkanSubpass::getInputAttachments() const noexcept
    {
        return inputAttachments;
    }

    const std::vector<VulkanSubpass::AttachmentReference>& VulkanSubpass::getColorAttachments() const noexcept
    {
        return colorAttachments;
    }

    const std::vector<VulkanSubpass::AttachmentReference>& VulkanSubpass::getResolveAttachments() const noexcept
    {
        return resolveAttachments;
    }

    const std::optional<VulkanSubpass::AttachmentReference>& VulkanSubpass::getDepthStencilAttachment() const noexcept
    {
        return depthStencilAttachment;
    }

    const std::vector<VulkanAttachment*>& VulkanSubpass::getPreserveAttachments() const noexcept
    {
        return preserveAttachments;
    }

    const std::vector<VulkanSubpass::Dependency>& VulkanSubpass::getDependencies() const noexcept
    {
        return dependencies;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void VulkanSubpass::setDescriptionFlags(const VkSubpassDescriptionFlags descriptionFlags)
    {
        layout->requireNonFinalized();
        flags = descriptionFlags;
    }

    void VulkanSubpass::setPipelineBindPoint(const VkPipelineBindPoint bindPoint)
    {
        layout->requireNonFinalized();
        pipelineBindPoint = bindPoint;
    }

    void VulkanSubpass::addInputAttachment(VulkanAttachment& attachment, const VkImageLayout imageLayout)
    {
        layout->requireNonFinalized();
        requireSameLayout(attachment);
        inputAttachments.emplace_back(AttachmentReference{.attachment = &attachment, .layout = imageLayout});
    }

    void VulkanSubpass::addColorAttachment(VulkanAttachment& attachment, const VkImageLayout imageLayout)
    {
        layout->requireNonFinalized();
        requireSameLayout(attachment);
        colorAttachments.emplace_back(AttachmentReference{.attachment = &attachment, .layout = imageLayout});
    }

    void VulkanSubpass::addResolveAttachment(VulkanAttachment& attachment, const VkImageLayout imageLayout)
    {
        layout->requireNonFinalized();
        requireSameLayout(attachment);
        resolveAttachments.emplace_back(AttachmentReference{.attachment = &attachment, .layout = imageLayout});
    }

    void VulkanSubpass::setDepthStencilAttachment(VulkanAttachment& attachment, const VkImageLayout imageLayout)
    {
        layout->requireNonFinalized();
        requireSameLayout(attachment);
        depthStencilAttachment = {.attachment = &attachment, .layout = imageLayout};
    }

    void VulkanSubpass::addPreserveAttachment(VulkanAttachment& attachment)
    {
        layout->requireNonFinalized();
        requireSameLayout(attachment);
        preserveAttachments.emplace_back(&attachment);
    }

    void VulkanSubpass::addExternalDependency(const bool           externalIsSrc,
                                              VkPipelineStageFlags srcStageMask,
                                              VkAccessFlags        srcAccessMask,
                                              VkPipelineStageFlags dstStageMask,
                                              VkAccessFlags        dstAccessMask,
                                              VkDependencyFlags    dependencyFlags)
    {
        layout->requireNonFinalized();
        dependencies.emplace_back(
          nullptr, externalIsSrc, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask, dependencyFlags);
    }

    void VulkanSubpass::addDependency(VulkanSubpass&       subpass,
                                      VkPipelineStageFlags srcStageMask,
                                      VkAccessFlags        srcAccessMask,
                                      VkPipelineStageFlags dstStageMask,
                                      VkAccessFlags        dstAccessMask,
                                      VkDependencyFlags    dependencyFlags)
    {
        layout->requireNonFinalized();
        requireSameLayout(subpass);
        dependencies.emplace_back(
          &subpass, false, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask, dependencyFlags);
    }

    void VulkanSubpass::requireSameLayout(const VulkanSubpass& subpass) const
    {
        if (&subpass.getLayout() != layout) throw SolError("Subpass is not part of the same layout as this subpass.");
    }

    void VulkanSubpass::requireSameLayout(const VulkanAttachment& attachment) const
    {
        if (&attachment.getLayout() != layout)
            throw SolError("Attachment is not part of the same layout as this subpass.");
    }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void VulkanSubpass::destroy() const
    {
        layout->requireNonFinalized();
        layout->destroySubpass(index);
    }
}  // namespace sol
