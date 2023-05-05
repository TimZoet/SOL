#include "sol-core/vulkan_image_view.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanImageView::VulkanImageView(SettingsPtr settingsPtr, const VkImageView vkImageView) :
        settings(std::move(settingsPtr)), imageView(vkImageView)
    {
    }

    VulkanImageView::~VulkanImageView() noexcept { vkDestroyImageView(getDevice().get(), imageView, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanImageViewPtr VulkanImageView::create(Settings settings)
    {
        auto view = createImpl(settings);
        return std::make_unique<VulkanImageView>(std::make_unique<Settings>(settings), view);
    }

    VulkanImageViewSharedPtr VulkanImageView::createShared(Settings settings)
    {
        auto view = createImpl(settings);
        return std::make_shared<VulkanImageView>(std::make_unique<Settings>(settings), view);
    }

    VkImageView VulkanImageView::createImpl(const Settings& settings)
    {
        // Prepare creation info.
        VkImageViewCreateInfo createInfo;
        createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext                           = nullptr;
        createInfo.flags                           = 0;
        createInfo.image                           = settings.image;
        createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format                          = settings.format;
        createInfo.components.r                    = settings.components.r;
        createInfo.components.g                    = settings.components.g;
        createInfo.components.b                    = settings.components.b;
        createInfo.components.a                    = settings.components.a;
        createInfo.subresourceRange.aspectMask     = settings.aspect;
        createInfo.subresourceRange.baseMipLevel   = 0;
        createInfo.subresourceRange.levelCount     = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount     = 1;

        // Create image view.
        VkImageView view;
        handleVulkanError(vkCreateImageView(settings.image().getDevice().get(), &createInfo, nullptr, &view));

        return view;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanImageView::Settings& VulkanImageView::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanImageView::getDevice() noexcept { return settings->image().getDevice(); }

    const VulkanDevice& VulkanImageView::getDevice() const noexcept { return settings->image().getDevice(); }

    const VkImageView& VulkanImageView::get() const noexcept { return imageView; }

    VulkanImage& VulkanImageView::getImage() noexcept { return settings->image(); }

    const VulkanImage& VulkanImageView::getImage() const noexcept { return settings->image(); }
}  // namespace sol
