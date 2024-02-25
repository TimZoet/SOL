#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/fwd.h"
#include "sol-memory/i_buffer.h"

namespace sol
{
    class Buffer : public IBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Buffer() = delete;

        Buffer(MemoryManager& memoryManager, VulkanQueueFamily& queueFamily, VulkanBufferPtr b);

        Buffer(const Buffer&) = delete;

        Buffer(Buffer&&) noexcept = default;

        ~Buffer() noexcept override;

        Buffer& operator=(const Buffer&) = delete;

        Buffer& operator=(Buffer&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanBuffer& getBuffer() override;

        [[nodiscard]] const VulkanBuffer& getBuffer() const override;

        [[nodiscard]] size_t getBufferSize() const noexcept override;

        [[nodiscard]] size_t getBufferOffset() const noexcept override;

        [[nodiscard]] bool isSubAllocation() const noexcept override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanBufferPtr buffer;
    };
}  // namespace sol
