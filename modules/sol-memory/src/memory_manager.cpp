#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <fstream>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/buffer.h"
#include "sol-memory/pool/free_at_once_memory_pool.h"
#include "sol-memory/pool/non_linear_memory_pool.h"
#include "sol-memory/pool/ring_buffer_memory_pool.h"
#include "sol-memory/pool/stack_memory_pool.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
    MemoryManager::MemoryManager(VulkanDevice& vkDevice) : IBufferAllocator(*this), device(&vkDevice)
    {
        initializeAllocator();
        initializeCommandPools();
    }

    MemoryManager::MemoryManager(VulkanMemoryAllocatorPtr alloc) :
        // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
        IBufferAllocator(*this), device(&alloc->getDevice()), allocator(std::move(alloc))
    {
        assert(allocator);
        initializeCommandPools();
    }

    MemoryManager::~MemoryManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& MemoryManager::getDevice() noexcept { return *device; }

    const VulkanDevice& MemoryManager::getDevice() const noexcept { return *device; }

    VulkanMemoryAllocator& MemoryManager::getAllocator() noexcept { return *allocator; }

    const VulkanMemoryAllocator& MemoryManager::getAllocator() const noexcept { return *allocator; }

    VulkanCommandPool& MemoryManager::getCommandPool(const VulkanQueueFamily& queueFamily) const
    {
        return getCommandPool(queueFamily.getIndex());
    }

    VulkanCommandPool& MemoryManager::getCommandPool(const uint32_t queueFamilyIndex) const
    {
        if (queueFamilyIndex > commandPools.size())
            throw SolError("Cannot retrieve command pool. Queue family index out of range.");
        return *commandPools.at(queueFamilyIndex);
    }

    VulkanQueue& MemoryManager::getQueue(const VulkanQueueFamily& queueFamily) const
    {
        return getQueue(queueFamily.getIndex());
    }

    VulkanQueue& MemoryManager::getQueue(const uint32_t queueFamilyIndex) const
    {
        if (computeQueue && computeQueue->getFamily().getIndex() == queueFamilyIndex) return *computeQueue;
        if (graphicsQueue && graphicsQueue->getFamily().getIndex() == queueFamilyIndex) return *graphicsQueue;
        if (transferQueue && transferQueue->getFamily().getIndex() == queueFamilyIndex) return *transferQueue;
        throw SolError("");
    }

    VulkanQueue& MemoryManager::getComputeQueue() const
    {
        if (!computeQueue) throw SolError("");
        return *computeQueue;
    }

    VulkanQueue& MemoryManager::getGraphicsQueue() const
    {
        if (!graphicsQueue) throw SolError("");
        return *graphicsQueue;
    }

    VulkanQueue& MemoryManager::getTransferQueue() const
    {
        if (!transferQueue) return getGraphicsQueue();
        return *transferQueue;
    }

    IBufferAllocator::Capabilities MemoryManager::getCapabilities() const noexcept { return Capabilities::Alignment; }

    IMemoryPool& MemoryManager::getMemoryPool(const std::string& name)
    {
        const auto it = memoryPools.find(name);
        if (it == memoryPools.end())
            throw SolError(std::format("A memory pool with this name ({}) does not exist.", name));
        return *it->second;
    }

    ////////////////////////////////////////////////////////////////
    // Initialization.
    ////////////////////////////////////////////////////////////////

    void MemoryManager::initializeAllocator()
    {
        VulkanMemoryAllocator::Settings settings;
        settings.device = getDevice();
        allocator       = VulkanMemoryAllocator::create(settings);
    }

    void MemoryManager::initializeCommandPools()
    {
        // Create a resettable command pool for each queue family.
        const auto size = getDevice().getPhysicalDevice().getQueueFamilies().size();
        commandPools.reserve(size);
        for (uint32_t i = 0; i < size; i++)
        {
            VulkanCommandPool::Settings commandPoolSettings;
            commandPoolSettings.device           = getDevice();
            commandPoolSettings.queueFamilyIndex = i;
            commandPoolSettings.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPools.emplace_back(VulkanCommandPool::create(commandPoolSettings));
        }
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MemoryManager::setComputeQueue(VulkanQueue& queue)
    {
        if (&queue.getDevice() != &getDevice())
            throw SolError("Cannot set compute queue. Queue is on a different device.");
        if (!queue.getFamily().supportsCompute())
            throw SolError("Cannot set compute queue. It does not support compute operations.");

        computeQueue = &queue;
    }

    void MemoryManager::setGraphicsQueue(VulkanQueue& queue)
    {
        if (&queue.getDevice() != &getDevice())
            throw SolError("Cannot set graphics queue. Queue is on a different device.");
        if (!queue.getFamily().supportsGraphics())
            throw SolError("Cannot set graphics queue. It does not support graphics operations.");

        graphicsQueue = &queue;
    }

    void MemoryManager::setTransferQueue(VulkanQueue& queue)
    {
        if (&queue.getDevice() != &getDevice())
            throw SolError("Cannot set transfer queue. Queue is on a different device.");
        if (!queue.getFamily().supportsTransfer())
            throw SolError("Cannot set transfer queue. It does not support transfer operations.");

        transferQueue = &queue;
    }

    ////////////////////////////////////////////////////////////////
    // Allocations.
    ////////////////////////////////////////////////////////////////

    IBufferPtr MemoryManager::allocateBufferImpl(const Allocation& alloc)
    {
        VulkanBuffer::Settings settings;
        settings.device             = getDevice();
        settings.size               = alloc.size;
        settings.bufferUsage        = alloc.bufferUsage;
        settings.sharingMode        = alloc.sharingMode;
        settings.allocator          = getAllocator();
        settings.vma.memoryUsage    = alloc.memoryUsage;
        settings.vma.requiredFlags  = alloc.requiredMemoryFlags;
        settings.vma.preferredFlags = alloc.preferredMemoryFlags;
        settings.vma.flags          = alloc.allocationFlags;
        return std::make_unique<Buffer>(*this, VulkanBuffer::create(settings));
    }

    IBufferPtr MemoryManager::allocateBufferImpl(const AllocationAligned& alloc)
    {
        VulkanBuffer::Settings settings;
        settings.device             = getDevice();
        settings.size               = alloc.size;
        settings.bufferUsage        = alloc.bufferUsage;
        settings.sharingMode        = alloc.sharingMode;
        settings.allocator          = getAllocator();
        settings.vma.memoryUsage    = alloc.memoryUsage;
        settings.vma.requiredFlags  = alloc.requiredMemoryFlags;
        settings.vma.preferredFlags = alloc.preferredMemoryFlags;
        settings.vma.flags          = alloc.allocationFlags;
        settings.vma.alignment      = alloc.alignment;
        return std::make_unique<Buffer>(*this, VulkanBuffer::create(settings));
    }

    ////////////////////////////////////////////////////////////////
    // Memory pools.
    ////////////////////////////////////////////////////////////////

    FreeAtOnceMemoryPool& MemoryManager::createFreeAtOnceMemoryPool(const std::string&      name,
                                                                    IMemoryPool::CreateInfo createInfo)
    {
        assert(createInfo.minBlocks <= createInfo.maxBlocks);
        assert(createInfo.maxBlocks > 0);
        createInfo.createFlags |= VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT;
        return createMemoryPool<FreeAtOnceMemoryPool>(name, createInfo);
    }

    NonLinearMemoryPool& MemoryManager::createNonLinearMemoryPool(const std::string&      name,
                                                                  IMemoryPool::CreateInfo createInfo)
    {
        assert(createInfo.minBlocks <= createInfo.maxBlocks);
        assert(createInfo.maxBlocks > 0);
        return createMemoryPool<NonLinearMemoryPool>(name, createInfo);
    }

    RingBufferMemoryPool& MemoryManager::createRingBufferMemoryPool(const std::string&      name,
                                                                    IMemoryPool::CreateInfo createInfo)
    {
        assert(createInfo.minBlocks <= 1);
        assert(createInfo.maxBlocks == 1);
        createInfo.createFlags |= VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT;
        return createMemoryPool<RingBufferMemoryPool>(name, createInfo);
    }

    StackMemoryPool& MemoryManager::createStackMemoryPool(const std::string& name, IMemoryPool::CreateInfo createInfo)
    {
        assert(createInfo.minBlocks <= createInfo.maxBlocks);
        assert(createInfo.maxBlocks > 0);
        createInfo.createFlags |= VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT;
        return createMemoryPool<StackMemoryPool>(name, createInfo);
    }

    ////////////////////////////////////////////////////////////////
    // Stats.
    ////////////////////////////////////////////////////////////////

    void MemoryManager::writeAllocatorStatsToFile(const std::filesystem::path& path) const
    {
        char* str = nullptr;
        vmaBuildStatsString(allocator->get(), &str, true);
        std::ofstream file(path);
        file << str;
        vmaFreeStatsString(allocator->get(), str);
    }

}  // namespace sol
