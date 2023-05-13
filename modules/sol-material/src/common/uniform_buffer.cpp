#include "sol-material/common/uniform_buffer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UniformBuffer::UniformBuffer(const MaterialLayout::SharingMethod shareMethod,
                                 const uint32_t                      setIndex,
                                 const uint32_t                      bindingIndex,
                                 const size_t                        slotCount,
                                 const size_t                        bufferSize,
                                 std::vector<VulkanBufferPtr>        bufferList) :
        sharingMethod(shareMethod),
        set(setIndex),
        binding(bindingIndex),
        inUse(slotCount, 0),
        size(bufferSize),
        buffers(std::move(bufferList))
    {
    }

    UniformBuffer::~UniformBuffer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MaterialLayout::SharingMethod UniformBuffer::getSharingMethod() const noexcept { return sharingMethod; }

    uint32_t UniformBuffer::getSet() const noexcept { return set; }

    uint32_t UniformBuffer::getBinding() const noexcept { return binding; }

    bool UniformBuffer::isFull() const noexcept { return activeSlots == inUse.size(); }

    VulkanBuffer& UniformBuffer::getBuffer(const size_t index) { return *buffers.at(index); }

    const VulkanBuffer& UniformBuffer::getBuffer(const size_t index) const { return *buffers.at(index); }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    std::pair<size_t, size_t> UniformBuffer::getFreeSlot()
    {
        if (activeSlots == inUse.size()) throw SolError("There are no free slots available in this UniformBuffer.");

        // Look for an unused slot.
        const auto freeIndex = std::ranges::find(inUse.begin(), inUse.end(), 0) - inUse.begin();

        // Mark as in use and increment number of active slots.
        inUse[freeIndex] = 1;
        activeSlots++;

        // Return free index and offset in buffer in bytes.
        return {freeIndex, freeIndex * size};
    }

    void UniformBuffer::map(const uint32_t index) const { buffers[index]->map(); }

    void UniformBuffer::unmap(const uint32_t index) const { buffers[index]->unmap(); }

    void UniformBuffer::flush(const uint32_t index) const { buffers[index]->flush(); }

}  // namespace sol
