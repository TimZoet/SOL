#include "sol-memory/buffer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Buffer::Buffer(MemoryManager& memoryManager, VulkanQueueFamily& queueFamily, VulkanBufferPtr b) :
        IBuffer(memoryManager, queueFamily), buffer(std::move(b))
    {
    }

    Buffer::~Buffer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanBuffer& Buffer::getBuffer() { return *buffer; }

    const VulkanBuffer& Buffer::getBuffer() const { return *buffer; }

    size_t Buffer::getBufferSize() const noexcept { return buffer->getSize(); }

    size_t Buffer::getBufferOffset() const noexcept { return 0; }

    bool Buffer::isSubAllocation() const noexcept { return false; }

}  // namespace sol
