#include "sol-core/vulkan_device_memory.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_physical_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanDeviceMemory::VulkanDeviceMemory(const Settings& set, const VkDeviceMemory vkMemory) :
        settings(set), memory(vkMemory)
    {
    }
#else
    VulkanDeviceMemory::VulkanDeviceMemory(const Settings& set, const VkDeviceMemory vkMemory) :
        device(&set.device()), memory(vkMemory)
    {
    }
#endif

    VulkanDeviceMemory::~VulkanDeviceMemory() noexcept { vkFreeMemory(getDevice().get(), memory, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDeviceMemoryPtr VulkanDeviceMemory::create(const Settings& settings)
    {
        const auto memory = createImpl(settings);
        return std::make_unique<VulkanDeviceMemory>(settings, memory);
    }

    VulkanDeviceMemorySharedPtr VulkanDeviceMemory::createShared(const Settings& settings)
    {
        const auto memory = createImpl(settings);
        return std::make_shared<VulkanDeviceMemory>(settings, memory);
    }

    VkDeviceMemory VulkanDeviceMemory::createImpl(const Settings& settings)
    {
        // Prepare memory allocation info.
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = settings.size;
        allocInfo.memoryTypeIndex =
          findMemoryType(settings.device().getPhysicalDevice(), settings.memoryTypeBits, settings.memoryPropertyFlags);

        // Allocate memory.
        VkDeviceMemory vkMemory;
        handleVulkanError(vkAllocateMemory(settings.device, &allocInfo, nullptr, &vkMemory));

        return vkMemory;
    }

    uint32_t VulkanDeviceMemory::findMemoryType(const VulkanPhysicalDevice& physicalDevice,
                                                const uint32_t              filter,
                                                const VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice.get(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if (filter & 1 << i && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw VulkanError("Did not find suitable memory type.");
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanDeviceMemory::Settings& VulkanDeviceMemory::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanDeviceMemory::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanDeviceMemory::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkDeviceMemory& VulkanDeviceMemory::get() const noexcept { return memory; }
}  // namespace sol