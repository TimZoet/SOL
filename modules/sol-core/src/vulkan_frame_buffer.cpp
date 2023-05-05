#include "sol-core/vulkan_frame_buffer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_render_pass.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanFramebuffer::VulkanFramebuffer(SettingsPtr settingsPtr, const VkFramebuffer vkFramebuffer) :
        settings(std::move(settingsPtr)), framebuffer(vkFramebuffer)
    {
    }

    VulkanFramebuffer::~VulkanFramebuffer() noexcept { vkDestroyFramebuffer(getDevice().get(), framebuffer, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanFramebufferPtr VulkanFramebuffer::create(Settings settings)
    {
        const auto framebuffer = createImpl(settings);
        return std::make_unique<VulkanFramebuffer>(std::make_unique<Settings>(std::move(settings)), framebuffer);
    }

    VulkanFramebufferSharedPtr VulkanFramebuffer::createShared(Settings settings)
    {
        const auto framebuffer = createImpl(settings);
        return std::make_shared<VulkanFramebuffer>(std::make_unique<Settings>(std::move(settings)), framebuffer);
    }

    VkFramebuffer VulkanFramebuffer::createImpl(const Settings& settings)
    {
        std::vector<VkImageView> attachments;
        attachments.reserve(settings.attachments.size());
        for (const auto* imageView : settings.attachments) attachments.emplace_back(imageView->get());

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = settings.renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments    = attachments.data();
        framebufferInfo.width           = settings.width;
        framebufferInfo.height          = settings.height;
        framebufferInfo.layers          = settings.layers;

        // Create framebuffer.
        VkFramebuffer framebuffer;
        handleVulkanError(
          vkCreateFramebuffer(settings.renderPass().getDevice().get(), &framebufferInfo, nullptr, &framebuffer));

        return framebuffer;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanFramebuffer::Settings& VulkanFramebuffer::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanFramebuffer::getDevice() noexcept { return settings->renderPass().getDevice(); }

    const VulkanDevice& VulkanFramebuffer::getDevice() const noexcept { return settings->renderPass().getDevice(); }

    const VkFramebuffer& VulkanFramebuffer::get() const noexcept { return framebuffer; }

    VkExtent2D VulkanFramebuffer::getExtent() const noexcept { return VkExtent2D{settings->width, settings->height}; }

}  // namespace sol