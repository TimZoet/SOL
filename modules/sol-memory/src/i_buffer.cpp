#include "sol-memory/i_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IBuffer::IBuffer(MemoryManager& memoryManager) : manager(&memoryManager) {}

    IBuffer::~IBuffer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& IBuffer::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& IBuffer::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& IBuffer::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& IBuffer::getMemoryManager() const noexcept { return *manager; }
}  // namespace sol
