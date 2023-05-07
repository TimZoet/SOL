#include "sol-core/vulkan_buffer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_device_memory.h"
#include "sol-core/vulkan_memory_allocator.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanBuffer::VulkanBuffer(const Settings&     set,
                               const VkBuffer      vkBuffer,
                               const VmaAllocation vmaAllocation,
                               void*               pMappedData) :
        settings(set), buffer(vkBuffer), memoryRequirements(), allocation(vmaAllocation), mappedData(pMappedData)
    {
        vkGetBufferMemoryRequirements(settings.device, buffer, &memoryRequirements);
    }
#else
    VulkanBuffer::VulkanBuffer(const Settings&     set,
                               const VkBuffer      vkBuffer,
                               const VmaAllocation vmaAllocation,
                               void*               pMappedData) :
        device(&set.device()),
        size(set.size),
        allocator(set.allocator ? &set.allocator() : nullptr),
        buffer(vkBuffer),
        memoryRequirements(),
        allocation(vmaAllocation),
        mappedData(pMappedData)
    {
        vkGetBufferMemoryRequirements(getDevice().get(), buffer, &memoryRequirements);
    }
#endif

    VulkanBuffer::~VulkanBuffer() noexcept
    {
        if (hasAllocator())
            vmaDestroyBuffer(getAllocator().get(), buffer, allocation);
        else
            vkDestroyBuffer(getDevice().get(), buffer, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanBufferPtr VulkanBuffer::create(const Settings& settings)
    {
        const auto [buffer, alloc, pMappedData] = createImpl(settings);
        return std::make_unique<VulkanBuffer>(settings, buffer, alloc, pMappedData);
    }

    VulkanBufferSharedPtr VulkanBuffer::createShared(const Settings& settings)
    {
        const auto [buffer, alloc, pMappedData] = createImpl(settings);
        return std::make_shared<VulkanBuffer>(settings, buffer, alloc, pMappedData);
    }

    std::tuple<VkBuffer, VmaAllocation, void*> VulkanBuffer::createImpl(const Settings& settings)
    {
        // Prepare buffer creation info.
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = settings.size;
        bufferInfo.usage       = settings.bufferUsage;
        bufferInfo.sharingMode = settings.sharingMode;

        VkBuffer      vkBuffer      = VK_NULL_HANDLE;
        VmaAllocation vmaAllocation = VK_NULL_HANDLE;
        void*         pMappedData   = nullptr;

        // Create buffer using VMA allocator.
        if (settings.allocator)
        {
            VmaAllocationInfo       vmaAllocationInfo;
            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage                   = settings.memoryUsage;
            allocInfo.requiredFlags           = settings.requiredFlags;
            allocInfo.preferredFlags          = settings.preferredFlags;
            allocInfo.flags                   = settings.flags;
            handleVulkanError(vmaCreateBuffer(
              settings.allocator, &bufferInfo, &allocInfo, &vkBuffer, &vmaAllocation, &vmaAllocationInfo));


            if (settings.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) pMappedData = vmaAllocationInfo.pMappedData;
        }
        // Create buffer.
        else { handleVulkanError(vkCreateBuffer(settings.device, &bufferInfo, nullptr, &vkBuffer)); }

        return std::make_tuple(vkBuffer, vmaAllocation, pMappedData);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanBuffer::Settings& VulkanBuffer::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanBuffer::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanBuffer::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    size_t VulkanBuffer::getSize() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.size;
#else
        return size;
#endif
    }

    bool VulkanBuffer::hasAllocator() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.allocator.valid();
#else
        return allocator;
#endif
    }

    VulkanMemoryAllocator& VulkanBuffer::getAllocator() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.allocator();
#else
        return *allocator;
#endif
    }

    const VulkanMemoryAllocator& VulkanBuffer::getAllocator() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.allocator();
#else
        return *allocator;
#endif
    }

    const VkBuffer& VulkanBuffer::get() const noexcept { return buffer; }

    const VkMemoryRequirements& VulkanBuffer::getMemoryRequirements() const noexcept { return memoryRequirements; }

    bool VulkanBuffer::isMapped() const noexcept { return mappedData != nullptr; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void VulkanBuffer::bindMemory(VulkanDeviceMemory& memory, const size_t offset)
    {
        if (deviceMemory) throw SolError("Cannot bind memory. Buffer was already bound.");
        if (hasAllocator()) throw SolError("Cannot bind memory. Buffer was created using a custom allocator.");

        handleVulkanError(vkBindBufferMemory(getDevice().get(), buffer, memory.get(), offset));
        deviceMemory = &memory;
    }

    // V.
    void VulkanBuffer::setData(const void* data, const size_t size) const { setData(data, size, 0); }

    // V.
    void VulkanBuffer::setData(const void* data, const size_t size, const size_t offset) const
    {
        if (!deviceMemory && !allocation) throw SolError("Cannot set buffer data. No device memory bound.");
        if (size + offset > getSize())
            throw SolError("Cannot set buffer data, size + offset exceeds total size of buffer.");
        if (size == 0) return;

        std::memcpy(static_cast<uint8_t*>(mappedData) + offset, data, size);
    }

    // V.
    void VulkanBuffer::setDataRepeated(const void* data, const size_t size, const size_t count) const
    {
        setDataRepeated(data, size, count, 0);
    }

    // V.
    void
      VulkanBuffer::setDataRepeated(const void* data, const size_t size, const size_t count, const size_t offset) const
    {
        if (!deviceMemory && !allocation) throw SolError("Cannot set buffer data. No device memory bound.");
        if (size * count + offset > getSize())
            throw SolError("Cannot set buffer data, size * count + offset exceeds total size of buffer.");
        if (count == 0 || size == 0) return;

        // Copy first element.
        std::memcpy(static_cast<std::byte*>(mappedData) + offset, data, size);

        size_t copyCount = 1;
        size_t remaining = count - 1;
        size_t element   = 1;

        // Copy in increasingly larger blocks until done.
        while (remaining)
        {
            std::memcpy(static_cast<std::byte*>(mappedData) + element * size + offset,
                        static_cast<std::byte*>(mappedData) + offset,
                        size * copyCount);

            element += copyCount;
            remaining -= copyCount;

            // Double copy size, limit by remaining.
            copyCount = std::min(copyCount * 2, remaining);
        }
    }

    // X.
    void VulkanBuffer::setData2D(const void*                   data,
                                 const std::array<uint32_t, 2> totalSize,
                                 const std::array<uint32_t, 2> regionOffset,
                                 const std::array<uint32_t, 2> regionSize) const
    {
        if (!deviceMemory && !allocation) throw SolError("Cannot set buffer data. No device memory bound.");
        if (totalSize[0] == 0 || totalSize[1] == 0) return;
        if (getSize() % (static_cast<size_t>(totalSize[0]) * totalSize[1]) != 0)
            throw SolError("Cannot set buffer data. Size of buffer is not divisible totalSize.x * totalSize.y.");
        if (regionOffset[0] + regionSize[0] > totalSize[0] || regionOffset[1] + regionSize[1] > totalSize[1])
            throw SolError("Cannot set buffer data. regionOffset + regionSize exceeds totalSize");
        if (regionSize[0] == 0 || regionSize[1] == 0) return;


        const auto elementSize = getSize() / (static_cast<size_t>(totalSize[0]) * totalSize[1]);
        const auto rowSize     = elementSize * regionSize[0];

        // Region width is equal to width of whole buffer, so we can do a single copy.
        if (regionSize[0] == totalSize[0])
        {
            std::memcpy(static_cast<std::byte*>(mappedData) + regionOffset[1] * rowSize, data, getSize());
            return;
        }

        // Copy per row.
        for (uint32_t y = regionOffset[1], yend = y + regionSize[1]; y < yend; y++)
        {
            const auto offset = (y * static_cast<size_t>(totalSize[0]) + regionOffset[0]) * elementSize;
            std::memcpy(static_cast<std::byte*>(mappedData) + offset, data, rowSize);
            data = static_cast<const std::byte*>(data) + rowSize;
        }
    }

    // X.
    void VulkanBuffer::setDataRepeated2D(const void*                   data,
                                         const std::array<uint32_t, 2> totalSize,
                                         const std::array<uint32_t, 2> regionOffset,
                                         const std::array<uint32_t, 2> regionSize) const
    {
        if (!deviceMemory && !allocation) throw SolError("Cannot set buffer data. No device memory bound.");
        if (totalSize[0] == 0 || totalSize[1] == 0) return;
        if (getSize() % (static_cast<size_t>(totalSize[0]) * totalSize[1]) != 0)
            throw SolError("Cannot set buffer data. Size of buffer is not divisible totalSize.x * totalSize.y.");
        if (regionOffset[0] + regionSize[0] > totalSize[0] || regionOffset[1] + regionSize[1] > totalSize[1])
            throw SolError("Cannot set buffer data. regionOffset + regionSize exceeds totalSize");
        if (regionSize[0] == 0 || regionSize[1] == 0) return;

        // Fill first row.
        const auto elementSize = getSize() / (static_cast<size_t>(totalSize[0]) * totalSize[1]);
        const auto offset      = (regionOffset[0] + static_cast<size_t>(regionOffset[1]) * totalSize[0]) * elementSize;
        setDataRepeated(data, elementSize, regionSize[0], offset);

        // Repeatedly copy first row.
        const auto rowSize = elementSize * totalSize[0];
        for (uint32_t y = 1; y < regionSize[1]; y++)
            std::memcpy(static_cast<std::byte*>(mappedData) + offset + rowSize * y,
                        static_cast<std::byte*>(mappedData) + offset,
                        rowSize);
    }


    void VulkanBuffer::map()
    {
        if (allocation)
            handleVulkanError(vmaMapMemory(getAllocator().get(), allocation, &mappedData));
        else
            handleVulkanError(vkMapMemory(getDevice().get(), deviceMemory->get(), 0, getSize(), 0, &mappedData));
    }

    void VulkanBuffer::unmap()
    {
        mappedData = nullptr;

        if (allocation)
            vmaUnmapMemory(getAllocator().get(), allocation);
        else
            vkUnmapMemory(getDevice().get(), deviceMemory->get());
    }

    void VulkanBuffer::flush() const
    {
        if (allocation)
            vmaFlushAllocation(getAllocator().get(), allocation, 0, VK_WHOLE_SIZE);
        else
            throw SolError("");
    }
}  // namespace sol
