#include "sol-memory/pool/i_memory_pool.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-core/vulkan_memory_pool.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "sol-memory/pool/memory_pool_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IMemoryPool::IMemoryPool(MemoryManager&      memoryManager,
                             std::string         poolName,
                             CreateInfo          createInfo,
                             VulkanMemoryPoolPtr memoryPool) :
        IBufferAllocator(memoryManager),
        name(std::move(poolName)),
        info(std::move(createInfo)),
        pool(std::move(memoryPool))
    {
    }

    IMemoryPool::~IMemoryPool() noexcept = default;

    VulkanMemoryPoolPtr IMemoryPool::create(VulkanMemoryAllocator& allocator, const CreateInfo& info)
    {
        VulkanMemoryPool::Settings settings;
        settings.allocator       = allocator;
        settings.flags           = info.createFlags;
        settings.bufferUsage     = info.bufferUsage;
        settings.memoryUsage     = info.memoryUsage;
        settings.requiredFlags   = info.requiredMemoryFlags;
        settings.preferredFlags  = info.preferredMemoryFlags;
        settings.allocationFlags = info.allocationFlags;
        settings.blockSize       = info.blockSize;
        settings.minBlocks       = info.minBlocks;
        settings.maxBlocks       = info.maxBlocks;
        return VulkanMemoryPool::create(settings);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VmaPoolCreateFlags IMemoryPool::getCreateFlags() const noexcept { return info.createFlags; }

    VkBufferUsageFlags IMemoryPool::getBufferUsage() const noexcept { return info.bufferUsage; }

    VmaMemoryUsage IMemoryPool::getMemoryUsage() const noexcept { return info.memoryUsage; }

    VkMemoryPropertyFlags IMemoryPool::getRequiredMemoryFlags() const noexcept { return info.requiredMemoryFlags; }

    VkMemoryPropertyFlags IMemoryPool::getPreferredMemoryFlags() const noexcept { return info.preferredMemoryFlags; }

    VmaAllocationCreateFlags IMemoryPool::getAllocationFlags() const noexcept { return info.allocationFlags; }

    size_t IMemoryPool::getBlockSize() const noexcept { return info.blockSize; }

    size_t IMemoryPool::getMinBlocks() const noexcept { return info.minBlocks; }

    size_t IMemoryPool::getMaxBlocks() const noexcept { return info.maxBlocks; }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    MemoryPoolBufferPtr IMemoryPool::allocateBuffer(AllocationInfo alloc, const OnAllocationFailure onFailure)
    {
        // If no explicit buffer usage was requested, take all buffer usage flags that this memory pool supports.
        if (alloc.bufferUsage == 0) alloc.bufferUsage = getBufferUsage();

        if ((alloc.bufferUsage & getBufferUsage()) != alloc.bufferUsage)
            throw SolError(
              std::format("Cannot allocate buffer from memory pool. Requested buffer usage flags {} do not match "
                          "supported flags {}.",
                          alloc.bufferUsage,
                          getBufferUsage()));

        return allocateMemoryPoolBuffer(alloc, onFailure);
    }

    MemoryPoolBufferPtr IMemoryPool::allocateBuffer(const size_t size, const OnAllocationFailure onFailure)
    {
        return allocateBuffer(AllocationInfo{.size = size}, onFailure);
    }

    IBufferPtr IMemoryPool::allocateBufferImpl(const IBufferAllocator::AllocationInfo& alloc,
                                               const OnAllocationFailure               onFailure)
    {
        if ((alloc.bufferUsage & getBufferUsage()) != alloc.bufferUsage)
            throw SolError(
              std::format("Cannot allocate buffer from memory pool. Requested buffer usage flags {} do not match "
                          "supported flags {}.",
                          alloc.bufferUsage,
                          getBufferUsage()));
        if ((alloc.memoryUsage & getMemoryUsage()) != alloc.memoryUsage)
            throw SolError(
              std::format("Cannot allocate buffer from memory pool. Requested memory usage flags {} do not match "
                          "supported flags {}.",
                          static_cast<uint32_t>(alloc.memoryUsage),
                          static_cast<uint32_t>(getMemoryUsage())));

        if ((alloc.requiredMemoryFlags & getRequiredMemoryFlags()) != alloc.requiredMemoryFlags)
            throw SolError(
              std::format("Cannot allocate buffer from memory pool. Requested required memory flags {} do not match "
                          "supported flags {}.",
                          alloc.requiredMemoryFlags,
                          getRequiredMemoryFlags()));

        if ((alloc.allocationFlags & getAllocationFlags()) != alloc.allocationFlags)
            throw SolError(std::format(
              "Cannot allocate buffer from memory pool. Requested required allocation flags {} do not match "
              "supported flags {}.",
              alloc.allocationFlags,
              getAllocationFlags()));

        const AllocationInfo alloc2{.size = alloc.size, .bufferUsage = alloc.bufferUsage, .alignment = alloc.alignment};
        return allocateMemoryPoolBuffer(alloc2, onFailure);
    }

    MemoryPoolBufferPtr IMemoryPool::allocateMemoryPoolBuffer(const AllocationInfo&     alloc,
                                                              const OnAllocationFailure onFailure)
    {
        if (onFailure == OnAllocationFailure::Wait && none(getCapabilities() & Capabilities::Wait))
            throw SolError("This memory pool does not support waiting.");

        do {
            auto buffer = allocateMemoryPoolBufferImpl(alloc, onFailure);
            if (buffer.has_value()) return std::move(*buffer);

            // Pool will count down at latch as well, signaling we can try allocating again.
            if (onFailure == OnAllocationFailure::Wait)
            {
                buffer.error()->arrive_and_wait();
                continue;
            }
            if (onFailure == OnAllocationFailure::Empty)
            {
                return nullptr;
            }

            throw SolError("Failed to allocate buffer from memory pool.");
        } while (true);
    }
}  // namespace sol
