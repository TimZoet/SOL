#include "sol-memory/pool/i_memory_pool.h"

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
                             const size_t             blckSize,
                             const size_t             minBlcks,
                             const size_t             maxBlcks) :
        manager(&memoryManager),
        name(std::move(poolName)),
        flags(createFlags),
        bufferUsage(bufUsage),
        memoryUsage(memUsage),
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

    VulkanDevice& IMemoryPool::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& IMemoryPool::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& IMemoryPool::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& IMemoryPool::getMemoryManager() const noexcept { return *manager; }

    VmaPoolCreateFlags IMemoryPool::getCreateFlags() const noexcept { return flags; }

    VkBufferUsageFlags IMemoryPool::getBufferUsage() const noexcept { return bufferUsage; }

    VmaMemoryUsage IMemoryPool::getMemoryUsage() const noexcept { return memoryUsage; }

    size_t IMemoryPool::getBlockSize() const noexcept { return blockSize; }

    size_t IMemoryPool::getMinBlocks() const noexcept { return minBlocks; }

    size_t IMemoryPool::getMaxBlocks() const noexcept { return maxBlocks; }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    MemoryPoolBufferPtr IMemoryPool::allocateBuffer(const size_t size) { return allocateBuffer(size, false); }

    MemoryPoolBufferPtr IMemoryPool::allocateBufferWithWait(const size_t size)
    {
        if (!any(getCapabilities() & Capabilities::Wait)) throw SolError("This memory pool does not support waiting.");
        return allocateBuffer(size, true);
    }

    MemoryPoolBufferPtr IMemoryPool::allocateBuffer(const size_t size, const bool waitOnOutOfMemory)
    {
        do {
            auto buffer = allocateBufferImpl(size, waitOnOutOfMemory);
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
        settings.allocator   = getMemoryManager().getAllocator();
        settings.flags       = getCreateFlags();
        settings.bufferUsage = getBufferUsage();
        settings.memoryUsage = getMemoryUsage();
        settings.blockSize   = getBlockSize();
        settings.minBlocks   = getMinBlocks();
        settings.maxBlocks   = getMaxBlocks();
        pool                 = VulkanMemoryPool::create(settings);
    }
}  // namespace sol
