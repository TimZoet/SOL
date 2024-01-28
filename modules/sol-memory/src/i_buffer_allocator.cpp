#include "sol-memory/i_buffer_allocator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-memory/i_buffer.h"
#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IBufferAllocator::IBufferAllocator(MemoryManager& memoryManager) : manager(&memoryManager) {}

    IBufferAllocator::~IBufferAllocator() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& IBufferAllocator::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& IBufferAllocator::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& IBufferAllocator::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& IBufferAllocator::getMemoryManager() const noexcept { return *manager; }

    VulkanQueueFamily& IBufferAllocator::getDefaultQueueFamily() noexcept
    {
        if (!defaultFamily) defaultFamily = &manager->getTransferQueue().getFamily();

        return *defaultFamily;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void IBufferAllocator::setDefaultQueueFamily(VulkanQueueFamily& queueFamily) noexcept
    {
        defaultFamily = &queueFamily;
    }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    IBufferPtr IBufferAllocator::allocateBuffer(const AllocationInfo& alloc, const OnAllocationFailure onFailure)
    {
        if (alloc.alignment > 0 && none(getCapabilities() & Capabilities::Alignment))
            throw SolError(
              std::format("Requested alignment is {}, but this buffer allocator does not support aligned allocation.",
                          alloc.alignment));

        if (onFailure == OnAllocationFailure::Wait && none(getCapabilities() & Capabilities::Wait))
            throw SolError("This buffer allocator does not support waiting.");

        return allocateBufferImpl(alloc, onFailure);
    }
}  // namespace sol
