#include "sol-core/vulkan_memory_pool.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_memory_allocator.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    VulkanMemoryPool::VulkanMemoryPool(const Settings& set, const VmaPool vmaPool) : settings(set), pool(vmaPool) {}

    VulkanMemoryPool::~VulkanMemoryPool() noexcept { vmaDestroyPool(settings.allocator, pool); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanMemoryPoolPtr VulkanMemoryPool::create(const Settings& settings)
    {
        auto alloc = createImpl(settings);
        return std::make_unique<VulkanMemoryPool>(settings, alloc);
    }

    VulkanMemoryPoolSharedPtr VulkanMemoryPool::createShared(const Settings& settings)
    {
        auto alloc = createImpl(settings);
        return std::make_shared<VulkanMemoryPool>(settings, alloc);
    }

    VmaPool VulkanMemoryPool::createImpl(const Settings& settings)
    {
        // Query for appropriate memory type.
        uint32_t                memIndex = 0;
        VmaAllocationCreateInfo aInfo    = {};
        aInfo.flags                      = settings.allocationFlags;
        aInfo.usage                      = settings.memoryUsage;
        aInfo.requiredFlags              = settings.requiredFlags;
        aInfo.preferredFlags             = settings.preferredFlags;

        if (settings.bufferUsage)
        {
            VkBufferCreateInfo bInfo{};
            bInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bInfo.usage = settings.bufferUsage;
            handleVulkanError(vmaFindMemoryTypeIndexForBufferInfo(settings.allocator, &bInfo, &aInfo, &memIndex));
        }
        else
        {
            VkImageCreateInfo iInfo{};
            iInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            iInfo.usage = settings.imageUsage;
            handleVulkanError(vmaFindMemoryTypeIndexForImageInfo(settings.allocator, &iInfo, &aInfo, &memIndex));
        }

        VmaPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.memoryTypeIndex = memIndex;
        poolCreateInfo.flags           = settings.flags;
        poolCreateInfo.blockSize       = settings.blockSize;
        poolCreateInfo.maxBlockCount   = settings.maxBlocks;
        poolCreateInfo.minBlockCount   = settings.minBlocks;

        // Create pool.
        VmaPool pool;
        handleVulkanError(vmaCreatePool(settings.allocator, &poolCreateInfo, &pool));

        return pool;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanMemoryPool::Settings& VulkanMemoryPool::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanMemoryPool::getDevice() noexcept { return settings.allocator().getDevice(); }

    const VulkanDevice& VulkanMemoryPool::getDevice() const noexcept { return settings.allocator().getDevice(); }

    const VmaPool& VulkanMemoryPool::get() const noexcept { return pool; }
}  // namespace sol
