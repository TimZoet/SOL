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
    class MemoryPoolBuffer : public IBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MemoryPoolBuffer() = delete;

        MemoryPoolBuffer(
          IMemoryPool& memoryPool, size_t id, VulkanBuffer& buffer, size_t bufferSize, size_t bufferOffset);

        MemoryPoolBuffer(const MemoryPoolBuffer&) = delete;

        MemoryPoolBuffer(MemoryPoolBuffer&&) noexcept;

        ~MemoryPoolBuffer() noexcept override;

        MemoryPoolBuffer& operator=(const MemoryPoolBuffer&) = delete;

        MemoryPoolBuffer& operator=(MemoryPoolBuffer&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] IMemoryPool& getMemoryPool() noexcept;

        [[nodiscard]] const IMemoryPool& getMemoryPool() const noexcept;

        [[nodiscard]] size_t getId() const noexcept;

        [[nodiscard]] VulkanBuffer& getBuffer() noexcept override;

        [[nodiscard]] const VulkanBuffer& getBuffer() const noexcept override;

        [[nodiscard]] size_t getBufferSize() const noexcept override;

        [[nodiscard]] size_t getBufferOffset() const noexcept override;

        [[nodiscard]] bool isSubAllocation() const noexcept override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IMemoryPool* pool = nullptr;

        size_t identifier = 0;

        VulkanBuffer* buffer = nullptr;

        size_t size = 0;

        size_t offset = 0;
    };
}  // namespace sol
