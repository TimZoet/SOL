#include "sol-texture/image_transfer/image_staging_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ImageStagingBuffer::ImageStagingBuffer(VulkanBuffer&                 buffer,
                                           const std::array<uint32_t, 2> regionOffset,
                                           const std::array<uint32_t, 2> regionSize) :
        stagingBuffer(&buffer), offset(regionOffset), size(regionSize)
    {
    }

    ImageStagingBuffer::~ImageStagingBuffer() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanBuffer& ImageStagingBuffer::getStagingBuffer() const noexcept { return *stagingBuffer; }

    std::array<uint32_t, 2> ImageStagingBuffer::getOffset() const noexcept { return offset; }

    std::array<uint32_t, 2> ImageStagingBuffer::getSize() const noexcept { return size; }

    std::array<uint32_t, 2> ImageStagingBuffer::toLocal(const std::array<uint32_t, 2> regionOffset) const noexcept
    {
        return {regionOffset[0] - offset[0], regionOffset[1] - offset[1]};
    }
}  // namespace sol
