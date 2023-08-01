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

    IMemoryPool::IMemoryPool(MemoryManager&           memoryManager,
                             std::string              poolName,
                             const VmaPoolCreateFlags createFlags,
                             const VkBufferUsageFlags bufUsage,
                             const VmaMemoryUsage     memUsage,
                             VkMemoryPropertyFlags    requiredMemFlags,
                             VkMemoryPropertyFlags    preferredMemFlags,
                             const size_t             blckSize,
                             const size_t             minBlcks,
                             const size_t             maxBlcks) :
        IBufferAllocator(memoryManager),
        name(std::move(poolName)),
        flags(createFlags),
        bufferUsage(bufUsage),
        memoryUsage(memUsage),
        requiredMemoryFlags(requiredMemFlags),
        preferredMemoryFlags(preferredMemFlags),
        blockSize(blckSize),
        minBlocks(minBlcks),
        maxBlocks(maxBlcks)
    {
        initialize();
    }

    IMemoryPool::~IMemoryPool() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VmaPoolCreateFlags IMemoryPool::getCreateFlags() const noexcept { return flags; }

    VkBufferUsageFlags IMemoryPool::getBufferUsage() const noexcept { return bufferUsage; }

    VmaMemoryUsage IMemoryPool::getMemoryUsage() const noexcept { return memoryUsage; }

    VkMemoryPropertyFlags IMemoryPool::getRequiredMemoryFlags() const noexcept { return requiredMemoryFlags; }

    VkMemoryPropertyFlags IMemoryPool::getPreferredMemoryFlags() const noexcept { return preferredMemoryFlags; }

    size_t IMemoryPool::getBlockSize() const noexcept { return blockSize; }

    size_t IMemoryPool::getMinBlocks() const noexcept { return minBlocks; }

    size_t IMemoryPool::getMaxBlocks() const noexcept { return maxBlocks; }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    MemoryPoolBufferPtr IMemoryPool::allocateBuffer(const size_t size) { return allocateMemoryPoolBuffer(size, false); }

    MemoryPoolBufferPtr IMemoryPool::allocateBufferWithWait(const size_t size)
    {
        if (none(getCapabilities() & Capabilities::Wait)) throw SolError("This memory pool does not support waiting.");
        return allocateMemoryPoolBuffer(size, true);
    }

    IBufferPtr IMemoryPool::allocateBufferImpl(const Allocation& alloc)
    {
        if ((alloc.bufferUsage & getBufferUsage()) != alloc.bufferUsage)
            throw SolError(
              std::format("Cannot allocate buffer from memory pool. Requested buffer usage flags {} do not match "
                          "supported flags {}.",
                          static_cast<uint32_t>(alloc.bufferUsage),
                          static_cast<uint32_t>(getBufferUsage())));
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
                          static_cast<uint32_t>(alloc.requiredMemoryFlags),
                          static_cast<uint32_t>(getRequiredMemoryFlags())));

        return allocateMemoryPoolBuffer(alloc.size, false);
    }

    IBufferPtr IMemoryPool::allocateBufferImpl(const AllocationAligned&) { throw SolError("Not supported."); }

    MemoryPoolBufferPtr IMemoryPool::allocateMemoryPoolBuffer(const size_t size, const bool waitOnOutOfMemory)
    {
        do {
            auto buffer = allocateMemoryPoolBufferImpl(size, waitOnOutOfMemory);
            if (buffer.has_value()) return std::move(*buffer);

            // Pool will count down at latch as well, signaling we can try allocating again.
            if (waitOnOutOfMemory)
            {
                buffer.error()->arrive_and_wait();
                continue;
            }

            throw SolError("Failed to allocate buffer from memory pool.");
        } while (true);
    }

    ////////////////////////////////////////////////////////////////
    // Initialization.
    ////////////////////////////////////////////////////////////////

    void IMemoryPool::initialize()
    {
        VulkanMemoryPool::Settings settings;
        settings.allocator      = getMemoryManager().getAllocator();
        settings.flags          = getCreateFlags();
        settings.bufferUsage    = getBufferUsage();
        settings.memoryUsage    = getMemoryUsage();
        settings.requiredFlags  = getRequiredMemoryFlags();
        settings.preferredFlags = getPreferredMemoryFlags();
        settings.blockSize      = getBlockSize();
        settings.minBlocks      = getMinBlocks();
        settings.maxBlocks      = getMaxBlocks();
        pool                    = VulkanMemoryPool::create(settings);
    }
}  // namespace sol