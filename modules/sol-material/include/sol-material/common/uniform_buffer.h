#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/material_layout_description.h"

namespace sol
{
    class UniformBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UniformBuffer() = default;

        UniformBuffer(MaterialLayoutDescription::SharingMethod shareMethod,
                      uint32_t                                 setIndex,
                      uint32_t                                 bindingIndex,
                      size_t                                   slotCount,
                      size_t                                   bufferSize,
                      std::vector<VulkanBufferPtr>             bufferList);

        UniformBuffer(const UniformBuffer&) = delete;

        UniformBuffer(UniformBuffer&&) = delete;

        ~UniformBuffer() noexcept;

        UniformBuffer& operator=(const UniformBuffer&) = delete;

        UniformBuffer& operator=(UniformBuffer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] MaterialLayoutDescription::SharingMethod getSharingMethod() const noexcept;

        [[nodiscard]] uint32_t getSet() const noexcept;

        [[nodiscard]] uint32_t getBinding() const noexcept;

        [[nodiscard]] bool isFull() const noexcept;

        /**
         * \brief Get a buffer.
         * \param index Data set.
         * \return Buffer.
         */
        [[nodiscard]] VulkanBuffer& getBuffer(size_t index);

        /**
         * \brief Get a buffer.
         * \param index Data set.
         * \return Buffer.
         */
        [[nodiscard]] const VulkanBuffer& getBuffer(size_t index) const;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get a free slot and mark it as in use.
         * \throws SolError Thrown if there are no more free slots.
         * \return offset in buffer in bytes.
         */
        [[nodiscard]] std::pair<size_t, size_t> getFreeSlot();

        /**
         * \brief Map buffer. Should only be called when not mapped.
         * \param index Data set.
         */
        void map(uint32_t index) const;

        /**
         * \brief Unmap buffer. Should only be called when mapped.
         * \param index Data set.
         */
        void unmap(uint32_t index) const;

        /**
         * \brief Flush buffer. Should only be called when not mapped.
         * \param index Data set.
         */
        void flush(uint32_t index) const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Sharing method.
         */
        MaterialLayoutDescription::SharingMethod sharingMethod = MaterialLayoutDescription::SharingMethod ::None;

        /**
         * \brief Set index.
         */
        uint32_t set = 0;

        /**
         * \brief Binding index. If this object is used for multiple bindings, index of the first binding.
         */
        uint32_t binding = 0;

        /**
         * \brief Number of active (occupied) slots.
         */
        size_t activeSlots = 0;

        /**
         * \brief Boolean array keeping track of which slots are in use.
         */
        std::vector<uint8_t> inUse;

        /**
         * \brief Size of a single buffer slot in bytes. (Total size of buffer is inUse.size() * size).
         */
        size_t size = 0;

        /**
         * \brief List of buffers, one for each data set.
         */
        std::vector<VulkanBufferPtr> buffers;
    };
}  // namespace sol