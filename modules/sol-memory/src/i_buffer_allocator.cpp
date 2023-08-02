#include "sol-memory/i_buffer_allocator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    IBufferPtr IBufferAllocator::allocateBuffer(const Allocation& alloc) { return allocateBufferImpl(alloc); }

    IBufferPtr IBufferAllocator::allocateBuffer(const AllocationAligned& alloc)
    {
        if (none(getCapabilities() & Capabilities::Alignment))
            throw SolError("This buffer allocator does not support aligned allocation.");
        return allocateBufferImpl(alloc);
    }
}  // namespace sol
