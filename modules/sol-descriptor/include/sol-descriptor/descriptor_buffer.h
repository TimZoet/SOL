#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/i_buffer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/fwd.h"

namespace sol
{
    class DescriptorBuffer final : public IBuffer
    {
    public:
        friend class Descriptor;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Settings
        {
            /**
             * \brief Memory manager.
             */
            MemoryManager* memoryManager = nullptr;

            /**
             * \brief Size of buffer in bytes.
             */
            size_t size = 0;

            // TODO: bufferUsage flags contains sampler as well. Is mixing samplers and other descriptors bad?

            /**
             * \brief Additional buffer usage flags. Should include at least one of the default values.
             */
            VkBufferUsageFlags usageflags =
              VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT | VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        DescriptorBuffer() = delete;

        explicit DescriptorBuffer(IBufferPtr b);

        DescriptorBuffer(const DescriptorBuffer&) = delete;

        DescriptorBuffer(DescriptorBuffer&&) = delete;

        ~DescriptorBuffer() noexcept override;

        DescriptorBuffer& operator=(const DescriptorBuffer&) = delete;

        DescriptorBuffer& operator=(DescriptorBuffer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] static DescriptorBufferPtr create(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanBuffer& getBuffer() override;

        [[nodiscard]] const VulkanBuffer& getBuffer() const override;

        [[nodiscard]] size_t getBufferSize() const noexcept override;

        [[nodiscard]] size_t getBufferOffset() const noexcept override;

        [[nodiscard]] bool isSubAllocation() const noexcept override;

        [[nodiscard]] VmaVirtualBlock getVirtualBlock() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Transactions.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] DescriptorPtr allocateDescriptor(const DescriptorLayout& layout);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        const VkPhysicalDeviceDescriptorBufferPropertiesEXT* properties = nullptr;

        IBufferPtr buffer;

        VmaVirtualBlock virtualBlock = VK_NULL_HANDLE;
    };
}  // namespace sol
