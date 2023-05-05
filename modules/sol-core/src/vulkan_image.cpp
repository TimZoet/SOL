#include "sol-core/vulkan_image.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_device_memory.h"
#include "sol-core/vulkan_memory_allocator.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanImage::VulkanImage(SettingsPtr settingsPtr, const VkImage vkImage, const VmaAllocation vmaAllocation) :
        settings(std::move(settingsPtr)), image(vkImage), memoryRequirements(), allocation(vmaAllocation)
    {
        if (!settings->isSwapchainImage) vkGetImageMemoryRequirements(settings->device, image, &memoryRequirements);
    }

    VulkanImage::~VulkanImage() noexcept
    {
        // Image may have been retrieved as part of a swapchain and is deallocated automatically.
        if (!settings->isSwapchainImage)
        {
            if (settings->allocator)
                vmaDestroyImage(settings->allocator, image, allocation);
            else
                vkDestroyImage(settings->device, image, nullptr);
        }
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanImagePtr VulkanImage::create(Settings settings)
    {
        auto [image, alloc] = createImpl(settings);
        return std::make_unique<VulkanImage>(std::make_unique<Settings>(settings), image, alloc);
    }

    VulkanImageSharedPtr VulkanImage::createShared(Settings settings)
    {
        auto [image, alloc] = createImpl(settings);
        return std::make_shared<VulkanImage>(std::make_unique<Settings>(settings), image, alloc);
    }

    std::pair<VkImage, VmaAllocation> VulkanImage::createImpl(const Settings& settings)
    {
        // Prepare creation info.
        VkImageCreateInfo createInfo;
        createInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.pNext                 = nullptr;
        createInfo.flags                 = 0;
        createInfo.imageType             = VK_IMAGE_TYPE_2D;
        createInfo.format                = settings.format;
        createInfo.extent.width          = settings.width;
        createInfo.extent.height         = settings.height;
        createInfo.extent.depth          = settings.depth;
        createInfo.mipLevels             = 1;
        createInfo.arrayLayers           = 1;
        createInfo.samples               = VK_SAMPLE_COUNT_1_BIT;
        createInfo.tiling                = settings.tiling;
        createInfo.usage                 = settings.imageUsage;
        createInfo.sharingMode           = settings.sharingMode;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = nullptr;
        createInfo.initialLayout         = settings.initialLayout;

        VkImage       vkImage       = VK_NULL_HANDLE;
        VmaAllocation vmaAllocation = VK_NULL_HANDLE;

        // Create image using VMA allocator.
        if (settings.allocator)
        {
            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage                   = settings.memoryUsage;
            allocInfo.requiredFlags           = settings.requiredFlags;
            allocInfo.preferredFlags          = settings.preferredFlags;
            allocInfo.flags                   = settings.flags;
            handleVulkanError(
              vmaCreateImage(settings.allocator, &createInfo, &allocInfo, &vkImage, &vmaAllocation, nullptr));
        }
        // Create image.
        else
        {
            handleVulkanError(vkCreateImage(settings.device, &createInfo, nullptr, &vkImage));
        }

        return {vkImage, vmaAllocation};
    }

    const VkMemoryRequirements& VulkanImage::getMemoryRequirements() const noexcept { return memoryRequirements; }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanImage::Settings& VulkanImage::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanImage::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanImage::getDevice() const noexcept { return settings->device(); }

    const VkImage& VulkanImage::get() const noexcept { return image; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void VulkanImage::bindMemory(VulkanDeviceMemory& memory, const size_t offset)
    {
        if (deviceMemory) throw SolError("Cannot bind memory. Memory was already bound.");
        if (settings->allocator) throw SolError("Cannot bind memory. Image was created using a custom allocator.");

        handleVulkanError(vkBindImageMemory(settings->device, image, memory.get(), offset));
        deviceMemory = &memory;
    }
}  // namespace sol