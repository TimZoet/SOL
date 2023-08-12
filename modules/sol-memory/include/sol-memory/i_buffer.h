#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

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

        IBuffer(MemoryManager& memoryManager, const VulkanQueueFamily& family);

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

        /**
         * \brief Get the queue family that currently owns this resource.
         * \return VulkanQueueFamily.
         */
        [[nodiscard]] const VulkanQueueFamily& getQueueFamily() const noexcept;

        [[nodiscard]] virtual VulkanBuffer& getBuffer() = 0;

        [[nodiscard]] virtual const VulkanBuffer& getBuffer() const = 0;

        [[nodiscard]] virtual size_t getBufferSize() const noexcept = 0;

        [[nodiscard]] virtual size_t getBufferOffset() const noexcept = 0;

        [[nodiscard]] virtual bool isSubAllocation() const noexcept = 0;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setQueueFamily(const VulkanQueueFamily& family) noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* manager = nullptr;

        /**
         * \brief Queue family that currently owns this buffer.
         */
        const VulkanQueueFamily* queueFamily = nullptr;
    };
}  // namespace sol
