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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanImageView::VulkanImageView(const Settings& set, const VkImageView vkImageView) :
        settings(set), imageView(vkImageView)
    {
    }
#else
    VulkanImageView::VulkanImageView(const Settings& set, const VkImageView vkImageView) :
        image(&set.image()), imageView(vkImageView)
    {
    }
#endif

    VulkanImageView::~VulkanImageView() noexcept { vkDestroyImageView(getDevice().get(), imageView, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanImageViewPtr VulkanImageView::create(const Settings& settings)
    {
        auto view = createImpl(settings);
        return std::make_unique<VulkanImageView>(settings, view);
    }

    VulkanImageViewSharedPtr VulkanImageView::createShared(const Settings& settings)
    {
        auto view = createImpl(settings);
        return std::make_shared<VulkanImageView>(settings, view);
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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanImageView::Settings& VulkanImageView::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanImageView::getDevice() noexcept { return getImage().getDevice(); }

    const VulkanDevice& VulkanImageView::getDevice() const noexcept { return getImage().getDevice(); }

    const VkImageView& VulkanImageView::get() const noexcept { return imageView; }

    VulkanImage& VulkanImageView::getImage() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.image();
#else
        return *image;
#endif
    }

    const VulkanImage& VulkanImageView::getImage() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.image();
#else
        return *image;
#endif
    }
}  // namespace sol
