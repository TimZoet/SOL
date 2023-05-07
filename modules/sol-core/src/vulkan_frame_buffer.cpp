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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanFramebuffer::VulkanFramebuffer(const Settings& set, const VkFramebuffer vkFramebuffer) :
        settings(set), framebuffer(vkFramebuffer)
    {
    }
#else
    VulkanFramebuffer::VulkanFramebuffer(const Settings& set, const VkFramebuffer vkFramebuffer) :
        renderPass(&set.renderPass()), extent(set.width, set.height), framebuffer(vkFramebuffer)
    {
    }
#endif

    VulkanFramebuffer::~VulkanFramebuffer() noexcept { vkDestroyFramebuffer(getDevice().get(), framebuffer, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanFramebufferPtr VulkanFramebuffer::create(const Settings& settings)
    {
        const auto framebuffer = createImpl(settings);
        return std::make_unique<VulkanFramebuffer>(settings, framebuffer);
    }

    VulkanFramebufferSharedPtr VulkanFramebuffer::createShared(const Settings& settings)
    {
        const auto framebuffer = createImpl(settings);
        return std::make_shared<VulkanFramebuffer>(settings, framebuffer);
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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanFramebuffer::Settings& VulkanFramebuffer::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanFramebuffer::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.renderPass().getDevice();
#else
        return renderPass->getDevice();
#endif
    }

    const VulkanDevice& VulkanFramebuffer::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.renderPass().getDevice();
#else
        return renderPass->getDevice();
#endif
    }

    const VkFramebuffer& VulkanFramebuffer::get() const noexcept { return framebuffer; }

    VkExtent2D VulkanFramebuffer::getExtent() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return VkExtent2D{settings.width, settings.height};
#else
        return extent;
#endif
    }
}  // namespace sol
