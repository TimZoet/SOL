#include "sol-descriptor/descriptor_buffer.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-descriptor/descriptor.h"
#include "sol-descriptor/descriptor_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    DescriptorBuffer::DescriptorBuffer(IBufferPtr b) :
        IBuffer(b->getMemoryManager(), b->getQueueFamily()), buffer(std::move(b))
    {
        properties = &getMemoryManager()
                        .getDevice()
                        .getPhysicalDevice()
                        .getProperties<VkPhysicalDeviceDescriptorBufferPropertiesEXT,
                                       VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT>();

        // Create virtual block.
        VmaVirtualBlockCreateInfo blockCreateInfo = {};
        // TODO: Look into making this optional.
        // blockCreateInfo.flags = VMA_VIRTUAL_BLOCK_CREATE_LINEAR_ALGORITHM_BIT;
        blockCreateInfo.size = buffer->getBufferSize();
        static_cast<void>(vmaCreateVirtualBlock(&blockCreateInfo, &virtualBlock));
    }

    DescriptorBuffer::~DescriptorBuffer() noexcept
    {
        vmaClearVirtualBlock(virtualBlock);
        vmaDestroyVirtualBlock(virtualBlock);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    DescriptorBufferPtr DescriptorBuffer::create(const Settings& settings)
    {
        // TODO: Using VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT. Could LINEAR_ be used to make things more efficient?

        const IBufferAllocator::AllocationInfo alloc{
          .size                 = settings.size,
          .bufferUsage          = settings.usageflags | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
          .requiredMemoryFlags  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
          .preferredMemoryFlags = 0,
          .allocationFlags      = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
          .alignment            = 0};

        auto buffer = settings.memoryManager->allocateBuffer(alloc, IBufferAllocator::OnAllocationFailure::Throw);

        return std::make_unique<DescriptorBuffer>(std::move(buffer));
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanBuffer& DescriptorBuffer::getBuffer() { return buffer->getBuffer(); }

    const VulkanBuffer& DescriptorBuffer::getBuffer() const { return buffer->getBuffer(); }

    size_t DescriptorBuffer::getBufferSize() const noexcept { return buffer->getBufferSize(); }

    size_t DescriptorBuffer::getBufferOffset() const noexcept { return buffer->getBufferOffset(); }

    bool DescriptorBuffer::isSubAllocation() const noexcept { return buffer->isSubAllocation(); }

    VmaVirtualBlock DescriptorBuffer::getVirtualBlock() const noexcept { return virtualBlock; }

    ////////////////////////////////////////////////////////////////
    // Transactions.
    ////////////////////////////////////////////////////////////////

    DescriptorPtr DescriptorBuffer::allocateDescriptor(const DescriptorLayout& layout)
    {
        layout.requireFinalized();

        const VmaVirtualAllocationCreateInfo info{.size      = layout.getLayoutSize(),
                                                  .alignment = properties->descriptorBufferOffsetAlignment,
                                                  .flags     = 0,
                                                  .pUserData = nullptr};
        VmaVirtualAllocation                 allocation;
        VkDeviceSize                         offset;
        handleVulkanError(vmaVirtualAllocate(virtualBlock, &info, &allocation, &offset));
        return std::make_unique<Descriptor>(*this, layout, allocation, offset);
    }
}  // namespace sol
