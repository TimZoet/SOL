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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanImage::VulkanImage(const Settings& set, const VkImage vkImage, const VmaAllocation vmaAllocation) :
        settings(set), image(vkImage), memoryRequirements(), allocation(vmaAllocation)
    {
        if (!isSwapchainImage()) vkGetImageMemoryRequirements(settings.device, image, &memoryRequirements);
    }
#else
    VulkanImage::VulkanImage(const Settings& set, const VkImage vkImage, const VmaAllocation vmaAllocation) :
        device(&set.device()),
        swapchainImage(set.isSwapchainImage),
        allocator(set.allocator ? &set.allocator() : nullptr),
        image(vkImage),
        memoryRequirements(),
        allocation(vmaAllocation)
    {
        if (!isSwapchainImage()) vkGetImageMemoryRequirements(device->get(), image, &memoryRequirements);
    }
#endif

    VulkanImage::~VulkanImage() noexcept
    {
        // Image may have been retrieved as part of a swapchain and is deallocated automatically.
        if (!isSwapchainImage())
        {
            if (hasAllocator())
                vmaDestroyImage(getAllocator().get(), image, allocation);
            else
                vkDestroyImage(getDevice().get(), image, nullptr);
        }
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanImagePtr VulkanImage::create(const Settings& settings)
    {
        auto [image, alloc] = createImpl(settings);
        return std::make_unique<VulkanImage>(settings, image, alloc);
    }

    VulkanImageSharedPtr VulkanImage::createShared(const Settings& settings)
    {
        auto [image, alloc] = createImpl(settings);
        return std::make_shared<VulkanImage>(settings, image, alloc);
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
        else { handleVulkanError(vkCreateImage(settings.device, &createInfo, nullptr, &vkImage)); }

        return {vkImage, vmaAllocation};
    }

    const VkMemoryRequirements& VulkanImage::getMemoryRequirements() const noexcept { return memoryRequirements; }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanImage::Settings& VulkanImage::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanImage::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanImage::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    bool VulkanImage::isSwapchainImage() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.isSwapchainImage;
#else
        return swapchainImage;
#endif
    }

    bool VulkanImage::hasAllocator() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.allocator.valid();
#else
        return allocator;
#endif
    }

    VulkanMemoryAllocator& VulkanImage::getAllocator() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.allocator();
#else
        return *allocator;
#endif
    }

    const VulkanMemoryAllocator& VulkanImage::getAllocator() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.allocator();
#else
        return *allocator;
#endif
    }

    const VkImage& VulkanImage::get() const noexcept { return image; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void VulkanImage::bindMemory(VulkanDeviceMemory& memory, const size_t offset)
    {
        if (deviceMemory) throw SolError("Cannot bind memory. Memory was already bound.");
        if (hasAllocator()) throw SolError("Cannot bind memory. Image was created using a custom allocator.");

        handleVulkanError(vkBindImageMemory(getDevice().get(), image, memory.get(), offset));
        deviceMemory = &memory;
    }
}  // namespace sol