#pragma once

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
    class IBuffer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IBuffer() = delete;

        explicit IBuffer(MemoryManager& memoryManager);

        IBuffer(const IBuffer&) = delete;

        IBuffer(IBuffer&&) noexcept = default;

        virtual ~IBuffer() noexcept;

        IBuffer& operator=(const IBuffer&) = delete;

        IBuffer& operator=(IBuffer&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        [[nodiscard]] virtual VulkanBuffer& getBuffer() = 0;

        [[nodiscard]] virtual const VulkanBuffer& getBuffer() const = 0;

        [[nodiscard]] virtual size_t getBufferSize() const noexcept = 0;

        [[nodiscard]] virtual size_t getBufferOffset() const noexcept = 0;

        [[nodiscard]] virtual bool isSubAllocation() const noexcept = 0;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* manager = nullptr;
    };
}  // namespace sol
