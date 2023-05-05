#include "sol-core/vulkan_swapchain.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <limits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_graphics_pipeline.h"
#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_surface.h"


namespace sol
{
    VulkanSwapchain::VulkanSwapchain(SettingsPtr          settingsPtr,
                                     const VkSwapchainKHR vkSwapchain,
                                     const CreateUtils    createUtils) :
        settings(std::move(settingsPtr)),
        swapchain(vkSwapchain),
        surfaceFormat(createUtils.surfaceFormat),
        presentMode(createUtils.presentMode),
        extent(createUtils.extent)
    {
        retrieveImages();
        createImageViews();
    }

    VulkanSwapchain::~VulkanSwapchain() noexcept { vkDestroySwapchainKHR(settings->device, swapchain, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSwapchainPtr VulkanSwapchain::create(Settings settings)
    {
        CreateUtils createUtils;
        const auto  swapchain = createImpl(settings, createUtils);
        return std::make_unique<VulkanSwapchain>(std::make_unique<Settings>(settings), swapchain, createUtils);
    }

    VulkanSwapchainSharedPtr VulkanSwapchain::createShared(Settings settings)
    {
        CreateUtils createUtils;
        const auto  swapchain = createImpl(settings, createUtils);
        return std::make_shared<VulkanSwapchain>(std::make_unique<Settings>(settings), swapchain, createUtils);
    }

    VkSwapchainKHR VulkanSwapchain::createImpl(const Settings& settings, CreateUtils& createUtils)
    {
        const auto& capabilities = settings.physicalDevice().getSwapchainSupportDetails().value().getCapabilities();

        // Prepare creation info.
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType          = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface        = settings.surface().get();
        createInfo.preTransform   = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.clipped        = VK_TRUE;
        createInfo.oldSwapchain   = VK_NULL_HANDLE;

        // Determine min image count.
        createUtils.imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && createUtils.imageCount > capabilities.maxImageCount)
            createUtils.imageCount = capabilities.maxImageCount;
        createInfo.minImageCount = createUtils.imageCount;

        // Determine surface format.
        createUtils.surfaceFormat   = selectSurfaceFormat(settings);
        createInfo.imageFormat      = createUtils.surfaceFormat.format;
        createInfo.imageColorSpace  = createUtils.surfaceFormat.colorSpace;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Determine present mode.
        createUtils.presentMode = selectPresentMode(settings);
        createInfo.presentMode  = createUtils.presentMode;

        // Determine extent.
        createUtils.extent     = selectExtent(settings);
        createInfo.imageExtent = createUtils.extent;

        // Determine image sharing mode.
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = nullptr;

        // Create swap chain.
        handleVulkanError(vkCreateSwapchainKHR(settings.device, &createInfo, nullptr, &createUtils.swapchain));

        return createUtils.swapchain;
    }

    VkSurfaceFormatKHR VulkanSwapchain::selectSurfaceFormat(const Settings& settings)
    {
        const auto& surfaceFormats = settings.physicalDevice().getSwapchainSupportDetails().value().getSurfaceFormats();
        // Look for 8-bit SRGB.
        for (const auto& f : surfaceFormats)
        {
            if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { return f; }
        }

        // Fall back to first available format.
        return surfaceFormats.front();
    }

    VkPresentModeKHR VulkanSwapchain::selectPresentMode(const Settings& settings)
    {
        // Look for preferred present mode.
        for (const auto& mode : settings.physicalDevice().getSwapchainSupportDetails().value().getPresentModes())
        {
            if (mode == settings.preferredPresentMode) return mode;
        }

        // Fall back to FIFO.
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapchain::selectExtent(const Settings& settings)
    {
        const auto& cap = settings.physicalDevice().getSwapchainSupportDetails().value().getCapabilities();

        // If != inf, directly use current extent.
        if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max()) return cap.currentExtent;

        // Otherwise, clamp to min/max extent.
        VkExtent2D extent = settings.extent;
        extent.width      = std::clamp(extent.width, cap.minImageExtent.width, cap.maxImageExtent.width);
        extent.height     = std::clamp(extent.height, cap.minImageExtent.height, cap.maxImageExtent.height);

        return extent;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanSwapchain::Settings& VulkanSwapchain::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanSwapchain::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanSwapchain::getDevice() const noexcept { return settings->device(); }

    const VkSwapchainKHR& VulkanSwapchain::get() const noexcept { return swapchain; }

    const VkSurfaceFormatKHR& VulkanSwapchain::getSurfaceFormat() const noexcept { return surfaceFormat; }

    const VkPresentModeKHR& VulkanSwapchain::getPresentMode() const noexcept { return presentMode; }

    const VkExtent2D& VulkanSwapchain::getExtent() const noexcept { return extent; }

    size_t VulkanSwapchain::getImageCount() const noexcept { return images.size(); }

    const std::vector<VulkanImagePtr>& VulkanSwapchain::getImages() const noexcept { return images; }

    const std::vector<VulkanImageViewPtr>& VulkanSwapchain::getImageViews() const noexcept { return imageViews; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void VulkanSwapchain::recreate()
    {
        imageViews.clear();
        images.clear();
        vkDestroySwapchainKHR(settings->device, swapchain, nullptr);

        settings->device().getPhysicalDevice().recreateSwapchainSupportDetails(&settings->surface.operator()());

        CreateUtils createUtils;
        swapchain     = createImpl(*settings, createUtils);
        surfaceFormat = createUtils.surfaceFormat;
        presentMode   = createUtils.presentMode;
        extent        = createUtils.extent;
        retrieveImages();
        createImageViews();
    }

    void VulkanSwapchain::retrieveImages()
    {
        if (!images.empty()) throw SolError("Images for this swapchain were already retrieved.");

        // Retrieve number of images.
        uint32_t imageCount;
        handleVulkanError(vkGetSwapchainImagesKHR(settings->device, swapchain, &imageCount, nullptr));

        // Retrieve image handles.
        std::vector<VkImage> vkImages(imageCount);
        handleVulkanError(vkGetSwapchainImagesKHR(settings->device, swapchain, &imageCount, vkImages.data()));

        // Create images from handles.
        images.reserve(imageCount);
        VulkanImage::Settings imageSettings;
        imageSettings.device           = settings->device;
        imageSettings.isSwapchainImage = true;
        for (auto img : vkImages)
            images.emplace_back(std::make_unique<VulkanImage>(
              std::make_unique<VulkanImage::Settings>(imageSettings), img, VK_NULL_HANDLE));
    }

    void VulkanSwapchain::createImageViews()
    {
        if (!imageViews.empty()) throw SolError("Image views for this swapchain were already created.");

        // Create image view for each image.
        imageViews.reserve(images.size());
        for (auto& image : images)
        {
            VulkanImageView::Settings viewSettings;
            viewSettings.image  = image;
            viewSettings.format = surfaceFormat.format;
            viewSettings.aspect = VK_IMAGE_ASPECT_COLOR_BIT;

            imageViews.emplace_back(VulkanImageView::create(viewSettings));
        }
    }
}  // namespace sol