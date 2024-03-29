#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>
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
    class IBufferAllocator
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Additional capabilities that can be supported by allocator implementations.
         * Certain methods can only be called if allocators advertise support for them. Performing
         * unsupported operations will result in runtime errors.
         */
        enum class Capabilities : uint32_t
        {
            None = 0,

            /**
             * \brief Allocator supports aligned allocation.
             */
            Alignment = 1,

            /**
             * \brief Allocator supports defragmentation of buffers allocated through it.
             */
            Defragmentation = 2,

            /**
             * \brief Allocator supports awaiting deallocation of other buffers
             * when trying to allocate a new buffer for which there is not enough space.
             */
            Wait = 4,

            /**
             * \brief Allocator supports suballocation of buffers.
             */
            SubAllocation = 8,
        };

        /**
         * \brief Action to take when an allocation fails due to an out of memory error.
         */
        enum class OnAllocationFailure : uint32_t
        {
            /**
             * \brief Throw an exception.
             */
            Throw = 0,

            /**
             * \brief Wait for space to become available. Requires the Wait capability.
             */
            Wait = 1,

            /**
             * \brief Return a nullptr.
             */
            Empty = 2
        };

        struct AllocationInfo
        {
            size_t                   size                 = 0;
            VkBufferUsageFlags       bufferUsage          = 0;
            VkSharingMode            sharingMode          = VK_SHARING_MODE_EXCLUSIVE;
            VmaMemoryUsage           memoryUsage          = VMA_MEMORY_USAGE_AUTO;
            VkMemoryPropertyFlags    requiredMemoryFlags  = 0;
            VkMemoryPropertyFlags    preferredMemoryFlags = 0;
            VmaAllocationCreateFlags allocationFlags      = 0;
            size_t                   alignment            = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IBufferAllocator() = delete;

        explicit IBufferAllocator(MemoryManager& memoryManager);

        IBufferAllocator(const IBufferAllocator&) = delete;

        IBufferAllocator(IBufferAllocator&&) noexcept = default;

        virtual ~IBufferAllocator() noexcept;

        IBufferAllocator& operator=(const IBufferAllocator&) = delete;

        IBufferAllocator& operator=(IBufferAllocator&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual VulkanDevice& getDevice() noexcept;

        [[nodiscard]] virtual const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        /**
         * \brief Get the default queue family newly allocated buffers are assigned to.
         * \return Queue family.
         */
        [[nodiscard]] VulkanQueueFamily& getDefaultQueueFamily() noexcept;

        [[nodiscard]] virtual Capabilities getCapabilities() const noexcept = 0;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDefaultQueueFamily(VulkanQueueFamily& queueFamily) noexcept;

        ////////////////////////////////////////////////////////////////
        // Allocations.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] IBufferPtr allocateBuffer(const AllocationInfo& alloc, OnAllocationFailure onFailure);

    protected:
        [[nodiscard]] virtual IBufferPtr allocateBufferImpl(const AllocationInfo& alloc,
                                                            OnAllocationFailure   onFailure) = 0;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* manager = nullptr;

        VulkanQueueFamily* defaultFamily = nullptr;
    };
}  // namespace sol
