#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

namespace sol
{
    class ImageStagingBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ImageStagingBuffer() = default;

        ImageStagingBuffer(VulkanBuffer&           buffer,
                           std::array<uint32_t, 2> regionOffset,
                           std::array<uint32_t, 2> regionSize);

        ImageStagingBuffer(const ImageStagingBuffer&) = default;

        ImageStagingBuffer(ImageStagingBuffer&&) = default;

        ~ImageStagingBuffer() noexcept;

        ImageStagingBuffer& operator=(const ImageStagingBuffer&) = default;

        ImageStagingBuffer& operator=(ImageStagingBuffer&&) = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanBuffer& getStagingBuffer() const noexcept;

        /**
         * \brief Get the offset of the region in the image this staging buffer covers.
         * \return Offset.
         */
        [[nodiscard]] std::array<uint32_t, 2> getOffset() const noexcept;

        /**
         * \brief Get the size of the region in the image this staging buffer covers.
         * \return Offset.
         */
        [[nodiscard]] std::array<uint32_t, 2> getSize() const noexcept;

        /**
         * \brief Convert an offset in the whole image to an offset inside of this staging buffer.
         * \param regionOffset Offset.
         * \return Local offset.
         */
        [[nodiscard]] std::array<uint32_t, 2> toLocal(std::array<uint32_t, 2> regionOffset) const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanBuffer* stagingBuffer = nullptr;

        std::array<uint32_t, 2> offset{};

        std::array<uint32_t, 2> size{};
    };
}  // namespace sol