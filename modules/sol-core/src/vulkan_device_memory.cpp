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

    VulkanDeviceMemory::VulkanDeviceMemory(SettingsPtr settingsPtr, const VkDeviceMemory vkMemory) :
        settings(std::move(settingsPtr)), memory(vkMemory)
    {
    }

    VulkanDeviceMemory::~VulkanDeviceMemory() noexcept { vkFreeMemory(settings->device, memory, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDeviceMemoryPtr VulkanDeviceMemory::create(Settings settings)
    {
        const auto memory = createImpl(settings);
        return std::make_unique<VulkanDeviceMemory>(std::make_unique<Settings>(settings), memory);
    }

    VulkanDeviceMemorySharedPtr VulkanDeviceMemory::createShared(Settings settings)
    {
        const auto memory = createImpl(settings);
        return std::make_shared<VulkanDeviceMemory>(std::make_unique<Settings>(settings), memory);
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

    const VulkanDeviceMemory::Settings& VulkanDeviceMemory::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanDeviceMemory::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanDeviceMemory::getDevice() const noexcept { return settings->device(); }

    const VkDeviceMemory& VulkanDeviceMemory::get() const noexcept { return memory; }
}  // namespace sol