#include "sol-mesh/geometry_buffer_allocator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_queue.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-memory/buffer.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/index_buffer.h"
#include "sol-mesh/vertex_buffer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GeometryBufferAllocator::GeometryBufferAllocator(MemoryManager& memoryManager) : IBufferAllocator(memoryManager) {}

    GeometryBufferAllocator::GeometryBufferAllocator(MemoryManager& memoryManager,
                                                     IBufferPtr     vtxBuffer,
                                                     IBufferPtr     idxBuffer,
                                                     const size_t   vtxSize,
                                                     const size_t   idxSize) :
        IBufferAllocator(memoryManager),
        vertexBuffer(std::move(vtxBuffer)),

        indexBuffer(std::move(idxBuffer)),
        vertexSize(vtxSize),
        indexSize(idxSize)
    {
        // Create virtual blocks. Use count instead of size in bytes, since we always allocate equally sized and aligned vertices and indices.
        VmaVirtualBlockCreateInfo blockCreateInfo = {};
        // TODO: Look into making this optional.
        // blockCreateInfo.flags = VMA_VIRTUAL_BLOCK_CREATE_LINEAR_ALGORITHM_BIT;
        blockCreateInfo.size = vertexBuffer->getBufferSize() / vertexSize;
        static_cast<void>(vmaCreateVirtualBlock(&blockCreateInfo, &virtualVertexBlock));
        blockCreateInfo.size = indexBuffer->getBufferSize() / indexSize;
        static_cast<void>(vmaCreateVirtualBlock(&blockCreateInfo, &virtualIndexBlock));
    }

    GeometryBufferAllocatorPtr GeometryBufferAllocator::create(Settings settings)
    {
        if (settings.strategy == Strategy::Separate)
            return std::make_unique<GeometryBufferAllocator>(settings.memoryManager);

        if (settings.vertexCount == 0 || settings.vertexSize == 0 || settings.indexCount == 0 ||
            settings.indexSize == 0)
            throw SolError("Cannot create GeometryBufferAllocator when vertex or index size or count is 0.");

        VulkanBuffer::Settings bSettings{.device      = settings.memoryManager.getDevice(),
                                         .size        = settings.vertexSize * settings.vertexCount,
                                         .bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                         .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                         .allocator   = settings.memoryManager.getAllocator(),
                                         .vma         = {.pool           = nullptr,
                                                         .memoryUsage    = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                         .requiredFlags  = 0,
                                                         .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                         .flags          = 0,
                                                         .alignment      = 0}};

        auto vtxBuffer = VulkanBuffer::create(bSettings);
        bSettings.size = settings.indexSize * settings.indexCount;
        bSettings.bufferUsage =
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        auto idxBuffer = VulkanBuffer::create(bSettings);

        return std::make_unique<GeometryBufferAllocator>(
          settings.memoryManager,
          std::make_unique<Buffer>(
            settings.memoryManager, settings.memoryManager.getTransferQueue().getFamily(), std::move(vtxBuffer)),
          std::make_unique<Buffer>(
            settings.memoryManager, settings.memoryManager.getTransferQueue().getFamily(), std::move(idxBuffer)),
          settings.vertexSize,
          settings.indexSize);
    }

    GeometryBufferAllocator::~GeometryBufferAllocator() noexcept
    {
        if (virtualVertexBlock)
        {
            vmaClearVirtualBlock(virtualVertexBlock);
            vmaClearVirtualBlock(virtualIndexBlock);
            vmaDestroyVirtualBlock(virtualVertexBlock);
            vmaDestroyVirtualBlock(virtualIndexBlock);
        }
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    IBufferAllocator::Capabilities GeometryBufferAllocator::getCapabilities() const noexcept
    {
        return Capabilities::None;
    }

    GeometryBufferAllocator::Strategy GeometryBufferAllocator::getStrategy() const noexcept
    {
        if (vertexBuffer) return Strategy::Global;
        return Strategy::Separate;
    }

    VmaVirtualBlock GeometryBufferAllocator::getVirtualVertexBlock() const noexcept { return virtualVertexBlock; }

    VmaVirtualBlock GeometryBufferAllocator::getVirtualIndexBlock() const noexcept { return virtualIndexBlock; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    VertexBufferPtr GeometryBufferAllocator::allocateVertexBuffer(const size_t count, const size_t size)
    {
        if (count == 0) throw SolError("Cannot allocate vertex buffer with 0 vertices.");

        // Do a suballocation in the global vertex buffer.
        if (getStrategy() == Strategy::Global)
        {
            const VmaVirtualAllocationCreateInfo info{.size = count, .alignment = 0, .flags = 0, .pUserData = nullptr};
            VmaVirtualAllocation                 allocation;
            VkDeviceSize                         offset;
            handleVulkanError(vmaVirtualAllocate(virtualVertexBlock, &info, &allocation, &offset));
            return std::make_unique<VertexBuffer>(*this, *vertexBuffer, count, vertexSize, allocation, offset);
        }

        if (size == 0) throw SolError("Cannot allocate vertex buffer with a size of 0 bytes");

        // Allocate a wholly separate vertex buffer.
        const VulkanBuffer::Settings settings{.device      = getDevice(),
                                              .size        = count * size,
                                              .bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                              .allocator   = getMemoryManager().getAllocator(),
                                              .vma         = {.pool           = nullptr,
                                                              .memoryUsage    = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                              .requiredFlags  = 0,
                                                              .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                              .flags          = 0,
                                                              .alignment      = 0}};

        auto buffer = VulkanBuffer::create(settings);
        return std::make_unique<VertexBuffer>(*this, std::move(buffer), count, size, getDefaultQueueFamily());
    }

    IndexBufferPtr GeometryBufferAllocator::allocateIndexBuffer(const size_t count, const size_t size)
    {
        if (count == 0) throw SolError("Cannot allocate index buffer with 0 indices.");

        // Do a suballocation in the global index buffer.
        if (getStrategy() == Strategy::Global)
        {
            const VmaVirtualAllocationCreateInfo info{.size = count, .alignment = 0, .flags = 0, .pUserData = nullptr};
            VmaVirtualAllocation                 allocation;
            VkDeviceSize                         offset;
            handleVulkanError(vmaVirtualAllocate(virtualIndexBlock, &info, &allocation, &offset));
            return std::make_unique<IndexBuffer>(*this, *indexBuffer, count, indexSize, allocation, offset);
        }

        if (size == 0) throw SolError("Cannot allocate index buffer with a size of 0 bytes");

        // Allocate a wholly separate index buffer.
        const VulkanBuffer::Settings settings{.device      = getDevice(),
                                              .size        = count * size,
                                              .bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                              .allocator   = getMemoryManager().getAllocator(),
                                              .vma         = {.pool           = nullptr,
                                                              .memoryUsage    = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                                                              .requiredFlags  = 0,
                                                              .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                              .flags          = 0,
                                                              .alignment      = 0}};

        auto buffer = VulkanBuffer::create(settings);
        return std::make_unique<IndexBuffer>(*this, std::move(buffer), count, size, getDefaultQueueFamily());
    }

    IBufferPtr GeometryBufferAllocator::allocateBufferImpl(const AllocationInfo&     alloc,
                                                           const OnAllocationFailure onFailure)
    {
        static_cast<void>(alloc);
        static_cast<void>(onFailure);
        throw SolError("Not yet implemented.");
    }

}  // namespace sol
