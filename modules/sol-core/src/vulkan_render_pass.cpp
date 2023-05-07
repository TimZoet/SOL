#include "sol-core/vulkan_render_pass.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <numeric>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_attachment.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_render_pass_layout.h"
#include "sol-core/vulkan_subpass.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanRenderPass::VulkanRenderPass(const Settings& set, const VkRenderPass vkRenderPass) :
        settings(set), renderPass(vkRenderPass)
    {
    }

    VulkanRenderPass::~VulkanRenderPass() noexcept { destroy(); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanRenderPassPtr VulkanRenderPass::create(const Settings& settings)
    {
        const auto renderPass = createImpl(settings);
        return std::make_unique<VulkanRenderPass>(settings, renderPass);
    }

    VulkanRenderPassSharedPtr VulkanRenderPass::createShared(const Settings& settings)
    {
        const auto renderPass = createImpl(settings);
        return std::make_shared<VulkanRenderPass>(settings, renderPass);
    }

    VkRenderPass VulkanRenderPass::createImpl(const Settings& settings)
    {
        const auto& layout = *settings.layout;
        layout.requireFinalized();

        // Prepare attachment descriptions.
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        attachmentDescriptions.reserve(layout.getAttachments().size());
        for (const auto& attachment : layout.getAttachments())
        {
            attachmentDescriptions.emplace_back(
              VkAttachmentDescription{.flags          = attachment->getDescriptionFlags(),
                                      .format         = attachment->getFormat(),
                                      .samples        = attachment->getSamples(),
                                      .loadOp         = attachment->getLoadOp(),
                                      .storeOp        = attachment->getStoreOp(),
                                      .stencilLoadOp  = attachment->getStencilLoadOp(),
                                      .stencilStoreOp = attachment->getStencilStoreOp(),
                                      .initialLayout  = attachment->getInitialLayout(),
                                      .finalLayout    = attachment->getFinalLayout()});
        }

        // Prepare subpass descriptions. Reserve space in the vectors to have stable pointers.
        std::vector<VkSubpassDescription>  subpassDescriptions;
        std::vector<VkAttachmentReference> attachmentReferences;
        std::vector<uint32_t>              preserveAttachments;
        subpassDescriptions.reserve(layout.getSubpasses().size());
        attachmentReferences.reserve(
          std::accumulate(layout.getSubpasses().cbegin(),
                          layout.getSubpasses().cend(),
                          static_cast<size_t>(0),
                          [](size_t sum, const auto& elem) { return sum + elem->getTotalAttachmentCount(); }));
        preserveAttachments.reserve(
          std::accumulate(layout.getSubpasses().cbegin(),
                          layout.getSubpasses().cend(),
                          static_cast<size_t>(0),
                          [](size_t sum, const auto& elem) { return sum + elem->getPreserveAttachments().size(); }));

        for (const auto& subpass : layout.getSubpasses())
        {
            // Setup all attachments. As stated above, the vectors had space reserved for stable pointers.
            // Here, we get the pointer just before adding new attachments to get the correct offsets.
            const VkAttachmentReference* inputAtt = nullptr;
            if (!subpass->getInputAttachments().empty())
            {
                inputAtt = attachmentReferences.data() + attachmentReferences.size();
                for (const auto& [a, l] : subpass->getInputAttachments())
                    attachmentReferences.emplace_back(a->getIndex(), l);
            }

            const VkAttachmentReference* colorAtt = nullptr;
            if (!subpass->getColorAttachments().empty())
            {
                colorAtt = attachmentReferences.data() + attachmentReferences.size();
                for (const auto& [a, l] : subpass->getColorAttachments())
                    attachmentReferences.emplace_back(a->getIndex(), l);
            }

            const VkAttachmentReference* resolveAtt = nullptr;
            if (!subpass->getResolveAttachments().empty())
            {
                resolveAtt = attachmentReferences.data() + attachmentReferences.size();
                for (const auto& [a, l] : subpass->getResolveAttachments())
                    attachmentReferences.emplace_back(a->getIndex(), l);
            }

            const VkAttachmentReference* depthAtt = nullptr;
            if (subpass->getDepthStencilAttachment())
            {
                depthAtt = attachmentReferences.data() + attachmentReferences.size();
                attachmentReferences.emplace_back(subpass->getDepthStencilAttachment()->attachment->getIndex(),
                                                  subpass->getDepthStencilAttachment()->layout);
            }

            const uint32_t* preserveAtt = nullptr;
            if (!subpass->getPreserveAttachments().empty())
            {
                preserveAtt = preserveAttachments.data() + attachmentReferences.size();
                for (const auto* attachment : subpass->getPreserveAttachments())
                    preserveAttachments.emplace_back(attachment->getIndex());
            }

            subpassDescriptions.emplace_back(VkSubpassDescription{
              .flags                   = subpass->getDescriptionFlags(),
              .pipelineBindPoint       = subpass->getPipelineBindPoint(),
              .inputAttachmentCount    = static_cast<uint32_t>(subpass->getInputAttachments().size()),
              .pInputAttachments       = inputAtt,
              .colorAttachmentCount    = static_cast<uint32_t>(subpass->getColorAttachments().size()),
              .pColorAttachments       = colorAtt,
              .pResolveAttachments     = resolveAtt,
              .pDepthStencilAttachment = depthAtt,
              .preserveAttachmentCount = static_cast<uint32_t>(subpass->getPreserveAttachments().size()),
              .pPreserveAttachments    = preserveAtt});
        }

        // Prepare subpass dependencies.
        std::vector<VkSubpassDependency> dependencies;
        for (const auto& subpass : layout.getSubpasses())
        {
            for (const auto& dep : subpass->getDependencies())
            {
                VkSubpassDependency dependency{};

                // If there is a dep.subpass, this subpass is automatically the destination.
                if (dep.subpass)
                {
                    dependency.srcSubpass = dep.subpass->getIndex();
                    dependency.dstSubpass = subpass->getIndex();
                }
                // Otherwise, this subpass is either the src or dst and should be linked to VK_SUBPASS_EXTERNAL on one end.
                else
                {
                    if (dep.externalIsSrc)
                    {
                        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                        dependency.dstSubpass = subpass->getIndex();
                    }
                    else
                    {
                        dependency.srcSubpass = subpass->getIndex();
                        dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
                    }
                }

                dependency.srcStageMask    = dep.srcStageMask;
                dependency.dstStageMask    = dep.dstStageMask;
                dependency.srcAccessMask   = dep.srcAccessMask;
                dependency.dstAccessMask   = dep.dstAccessMask;
                dependency.dependencyFlags = dep.dependencyFlags;

                dependencies.emplace_back(dependency);
            }
        }

        // Prepare create info.
        VkRenderPassCreateInfo info;
        info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.pNext           = nullptr;
        info.flags           = 0;
        info.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        info.pAttachments    = attachmentDescriptions.data();
        info.subpassCount    = static_cast<uint32_t>(subpassDescriptions.size());
        info.pSubpasses      = subpassDescriptions.data();
        info.dependencyCount = static_cast<uint32_t>(dependencies.size());
        info.pDependencies   = dependencies.data();

        // Create render pass.
        VkRenderPass renderPass;
        handleVulkanError(vkCreateRenderPass(settings.device, &info, nullptr, &renderPass));

        return renderPass;
    }

    void VulkanRenderPass::destroy()
    {
        // Destroy render pass.
        vkDestroyRenderPass(getDevice().get(), renderPass, nullptr);

        // Clear handle.
        renderPass = VK_NULL_HANDLE;
    }

    void VulkanRenderPass::recreate()
    {
        if (renderPass != VK_NULL_HANDLE)
            throw SolError("Cannot recreate VulkanRenderPass before explicitly destroying it.");

        const auto p = createImpl(settings);
        renderPass   = p;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanRenderPass::Settings& VulkanRenderPass::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanRenderPass::getDevice() noexcept { return settings.device(); }

    const VulkanDevice& VulkanRenderPass::getDevice() const noexcept { return settings.device(); }

    const VkRenderPass& VulkanRenderPass::get() const noexcept { return renderPass; }
}  // namespace sol
