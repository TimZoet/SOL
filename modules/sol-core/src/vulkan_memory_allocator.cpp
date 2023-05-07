#include "sol-core/vulkan_memory_allocator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_physical_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanMemoryAllocator::VulkanMemoryAllocator(const Settings& set, const VmaAllocator vmaAllocator) :
        settings(set), allocator(vmaAllocator)
    {
    }

    VulkanMemoryAllocator::~VulkanMemoryAllocator() noexcept { vmaDestroyAllocator(allocator); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanMemoryAllocatorPtr VulkanMemoryAllocator::create(const Settings& settings)
    {
        auto alloc = createImpl(settings);
        return std::make_unique<VulkanMemoryAllocator>(settings, alloc);
    }

    VulkanMemoryAllocatorSharedPtr VulkanMemoryAllocator::createShared(const Settings& settings)
    {
        auto alloc = createImpl(settings);
        return std::make_shared<VulkanMemoryAllocator>(settings, alloc);
    }

    VmaAllocator VulkanMemoryAllocator::createImpl(const Settings& settings)
    {
        const auto& physicalDevice = settings.device().getPhysicalDevice();
        const auto& instance       = physicalDevice.getInstance();

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion       = Version::getApiVersion();
        allocatorInfo.instance               = instance.get();
        allocatorInfo.physicalDevice         = physicalDevice.get();
        allocatorInfo.device                 = settings.device;

        // Create allocator.
        VmaAllocator alloc;
        handleVulkanError(vmaCreateAllocator(&allocatorInfo, &alloc));

        return alloc;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanMemoryAllocator::Settings& VulkanMemoryAllocator::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanMemoryAllocator::getDevice() noexcept { return settings.device(); }

    const VulkanDevice& VulkanMemoryAllocator::getDevice() const noexcept { return settings.device(); }

    const VmaAllocator& VulkanMemoryAllocator::get() const noexcept { return allocator; }
}  // namespace sol
