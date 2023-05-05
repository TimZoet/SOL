#include "sol-core/vulkan_render_pass_layout.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_attachment.h"
#include "sol-core/vulkan_subpass.h"
#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanRenderPassLayout::VulkanRenderPassLayout() = default;

    VulkanRenderPassLayout::VulkanRenderPassLayout(const VulkanRenderPassLayout& other) { *this = other; }

    VulkanRenderPassLayout::~VulkanRenderPassLayout() noexcept = default;

    VulkanRenderPassLayout& VulkanRenderPassLayout::operator=(const VulkanRenderPassLayout& other)
    {
        for (const auto& a : other.attachments) attachments.emplace_back(std::make_unique<VulkanAttachment>(*a));
        for (const auto& s : other.subpasses) subpasses.emplace_back(std::make_unique<VulkanSubpass>(*s));
        finalized = other.finalized;
        return *this;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool VulkanRenderPassLayout::isFinalized() const noexcept { return finalized; }

    const std::vector<VulkanAttachmentPtr>& VulkanRenderPassLayout::getAttachments() const noexcept
    {
        return attachments;
    }

    const std::vector<VulkanSubpassPtr>& VulkanRenderPassLayout::getSubpasses() const noexcept { return subpasses; }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    void VulkanRenderPassLayout::finalize()
    {
        requireNonFinalized();

        finalized = true;
    }

    void VulkanRenderPassLayout::requireFinalized() const
    {
        if (!finalized) throw SolError("VulkanRenderPassLayout was not yet finalized.");
    }

    void VulkanRenderPassLayout::requireNonFinalized() const
    {
        if (finalized) throw SolError("VulkanRenderPassLayout was already finalized.");
    }

    VulkanAttachment& VulkanRenderPassLayout::createAttachment()
    {
        requireNonFinalized();

        return *attachments.emplace_back(
          std::make_unique<VulkanAttachment>(*this, static_cast<uint32_t>(attachments.size())));
    }

    VulkanSubpass& VulkanRenderPassLayout::createSubpass()
    {
        requireNonFinalized();

        return *subpasses.emplace_back(std::make_unique<VulkanSubpass>(*this, static_cast<uint32_t>(subpasses.size())));
    }

    void VulkanRenderPassLayout::destroyAttachment(const uint32_t index)
    {
        if (index >= attachments.size()) throw SolError("Cannot remove attachment, index out of range.");

        // Remove attachment.
        attachments.erase(attachments.begin() + index);

        // Shift all later attachments.
        for (const auto& attachment : attachments | std::views::drop(index)) attachment->index -= 1;
    }

    void VulkanRenderPassLayout::destroySubpass(const uint32_t index)
    {
        if (index >= subpasses.size()) throw SolError("Cannot remove subpass, index out of range.");

        // Remove subpass.
        subpasses.erase(subpasses.begin() + index);

        // Shift all later subpasses.
        for (const auto& subpass : subpasses | std::views::drop(index)) subpass->index -= 1;
    }
}  // namespace sol
