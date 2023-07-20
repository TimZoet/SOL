#include "pathtracing/framebuffers.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_frame_buffer.h"
#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_render_pass.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-texture/image2d.h"
#include "sol-texture/texture_manager.h"
#include "sol-texture/texture2d.h"

std::vector<sol::Texture2D*> createGuiRenderTextures(sol::TextureManager&          textureManager,
                                                     const VkFormat                colorFormat,
                                                     const std::array<uint32_t, 2> size,
                                                     const size_t                  count)
{
    std::vector<sol::Texture2D*> textures;

    for (size_t i = 0; i < count; i++)
    {
        auto& image = textureManager.createImage2D(
          colorFormat, size, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        auto& texture = textureManager.createTexture2D(image);
        textures.emplace_back(&texture);
    }

    return textures;
}

std::vector<sol::VulkanFramebufferPtr> createGuiFramebuffers(sol::VulkanRenderPass&              renderPass,
                                                             const std::vector<sol::Texture2D*>& textures)
{
    std::vector<sol::VulkanFramebufferPtr> framebuffers;

    for (auto* tex : textures)
    {
        sol::VulkanFramebuffer::Settings framebufferSettings;
        framebufferSettings.renderPass = renderPass;
        framebufferSettings.attachments.emplace_back(tex->getImageView());
        framebufferSettings.width  = tex->getImage()->getSize()[0];
        framebufferSettings.height = tex->getImage()->getSize()[1];
        framebuffers.emplace_back(sol::VulkanFramebuffer::create(framebufferSettings));
    }

    return framebuffers;
}

std::vector<sol::Texture2D*> createViewerRenderTextures(sol::TextureManager&          textureManager,
                                                        const VkFormat                colorFormat,
                                                        const std::array<uint32_t, 2> size,
                                                        const size_t                  count)
{
    std::vector<sol::Texture2D*> textures;

    for (size_t i = 0; i < count; i++)
    {
        auto& image = textureManager.createImage2D(
          colorFormat, size, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        auto& texture = textureManager.createTexture2D(image);
        textures.emplace_back(&texture);
    }

    return textures;
}

std::vector<sol::VulkanFramebufferPtr> createViewerFramebuffers(sol::VulkanRenderPass&              renderPass,
                                                                const std::vector<sol::Texture2D*>& textures)
{
    std::vector<sol::VulkanFramebufferPtr> framebuffers;
    auto&                                  textureManager = textures.front()->getTextureManager();

    for (auto* tex : textures)
    {
        auto& depthImage = textureManager.createImage2D(
          VK_FORMAT_D32_SFLOAT, tex->getImage()->getSize(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
        depthImage.setTargetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        depthImage.setAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT);
        auto& depthTexture = textureManager.createTexture2D(depthImage);

        sol::VulkanFramebuffer::Settings framebufferSettings;
        framebufferSettings.renderPass = renderPass;
        framebufferSettings.attachments.emplace_back(tex->getImageView());
        framebufferSettings.attachments.emplace_back(depthTexture.getImageView());
        framebufferSettings.width  = tex->getImage()->getSize()[0];
        framebufferSettings.height = tex->getImage()->getSize()[1];
        framebuffers.emplace_back(sol::VulkanFramebuffer::create(framebufferSettings));
    }

    return framebuffers;
}

std::vector<sol::VulkanFramebufferPtr> createDisplayFramebuffers(sol::VulkanRenderPass&      renderPass,
                                                                 const sol::VulkanSwapchain& swapchain)
{
    std::vector<sol::VulkanFramebufferPtr> framebuffers;

    for (size_t i = 0; i < swapchain.getImageCount(); i++)
    {
        sol::VulkanFramebuffer::Settings framebufferSettings;
        framebufferSettings.renderPass = renderPass;
        framebufferSettings.attachments.emplace_back(swapchain.getImageViews()[i].get());
        framebufferSettings.width  = swapchain.getExtent().width;
        framebufferSettings.height = swapchain.getExtent().height;
        framebuffers.emplace_back(sol::VulkanFramebuffer::create(framebufferSettings));
    }

    return framebuffers;
}