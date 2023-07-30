#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cstdint>
#include <string>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/fwd.h"

namespace sol
{
    class MemoryPoolBuffer
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

        ~MemoryPoolBuffer() noexcept;

        MemoryPoolBuffer& operator=(const MemoryPoolBuffer&) = delete;

        MemoryPoolBuffer& operator=(MemoryPoolBuffer&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] IMemoryPool& getMemoryPool() noexcept;

        [[nodiscard]] const IMemoryPool& getMemoryPool() const noexcept;

        [[nodiscard]] size_t getId() const noexcept;

        [[nodiscard]] VulkanBuffer& getBuffer() noexcept;

        [[nodiscard]] const VulkanBuffer& getBuffer() const noexcept;

        [[nodiscard]] size_t getBufferSize() const noexcept;

        [[nodiscard]] size_t getBufferOffset() const noexcept;

        [[nodiscard]] bool isSubAllocation() const noexcept;

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
