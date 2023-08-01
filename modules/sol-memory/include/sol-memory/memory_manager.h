#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <unordered_map>
#include <vector>

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
#include "sol-memory/i_buffer_allocator.h"

namespace sol
{
    class MemoryManager : public IBufferAllocator
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MemoryManager() = delete;

        /**
         * \brief Construct a new MemoryManager. Will automatically initialize a VulkanMemoryAllocator with all default settings.
         * \param vkDevice VulkanDevice.
         */
        explicit MemoryManager(VulkanDevice& vkDevice);

        /**
         * \brief Construct a new MemoryManager with a user supplied allocator.
         * \param alloc VulkanMemoryAllocator.
         */
        explicit MemoryManager(VulkanMemoryAllocatorPtr alloc);

        MemoryManager(const MemoryManager&) = delete;

        MemoryManager(MemoryManager&&) noexcept = default;

        ~MemoryManager() noexcept override;

        MemoryManager& operator=(const MemoryManager&) = delete;

        MemoryManager& operator=(MemoryManager&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept override;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept override;

        [[nodiscard]] VulkanMemoryAllocator& getAllocator() noexcept;

        [[nodiscard]] const VulkanMemoryAllocator& getAllocator() const noexcept;

        [[nodiscard]] VulkanCommandPool& getCommandPool(const VulkanQueueFamily& queueFamily) const;

        [[nodiscard]] VulkanCommandPool& getCommandPool(uint32_t queueFamilyIndex) const;

        [[nodiscard]] VulkanQueue& getQueue(const VulkanQueueFamily& queueFamily) const;

        [[nodiscard]] VulkanQueue& getQueue(uint32_t queueFamilyIndex) const;

        [[nodiscard]] VulkanQueue& getComputeQueue() const;

        [[nodiscard]] VulkanQueue& getGraphicsQueue() const;

        [[nodiscard]] VulkanQueue& getTransferQueue() const;

        [[nodiscard]] Capabilities getCapabilities() const noexcept override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setComputeQueue(VulkanQueue& queue);

        void setGraphicsQueue(VulkanQueue& queue);

        void setTransferQueue(VulkanQueue& queue);

        ////////////////////////////////////////////////////////////////
        // Allocations.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] IBufferPtr allocateBufferImpl(const Allocation& alloc) override;

        [[nodiscard]] IBufferPtr allocateBufferImpl(const AllocationAligned& alloc) override;

        ////////////////////////////////////////////////////////////////
        // Memory pools.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new memory pool.
         * \tparam Pool Pool type. Must derive from IMemoryPool.
         * \tparam Args Additional parameter types passed to the constructor.
         * \param name Unique pool name.
         * \param args Additional parameter values passed to the constructor.
         * \return New memory pool.
         */
        template<std::derived_from<IMemoryPool> Pool, typename... Args>
            requires std::constructible_from<Pool, MemoryManager&, std::string, Args...>
        Pool& createMemoryPool(const std::string& name, Args&&... args)
        {
            auto  pool    = std::make_unique<Pool>(*this, name, std::forward<Args>(args)...);
            auto& poolRef = *pool;
            createMemoryPoolImpl(name, std::move(pool));
            return poolRef;
        }

        /**
         * \brief Create a new FreeAtOnceMemoryPool.
         * \param name Unique pool name.
         * \param bufferUsage Buffer usage flags.
         * \param memoryUsage Memory usage flags.
         * \param blockSize Size of memory blocks in bytes.
         * \param minBlocks Minimum number of memory blocks. If > 0, these blocks are preallocated.
         * \param maxBlocks Maximum number of memory blocks.
         * \return New FreeAtOnceMemoryPool.
         */
        FreeAtOnceMemoryPool& createFreeAtOnceMemoryPool(const std::string&    name,
                                                         VkBufferUsageFlags    bufferUsage,
                                                         VmaMemoryUsage        memoryUsage,
                                                         VkMemoryPropertyFlags requiredMemoryFlags,
                                                         VkMemoryPropertyFlags preferredMemoryFlags,
                                                         size_t                blockSize,
                                                         size_t                minBlocks,
                                                         size_t                maxBlocks);

        /**
         * \brief Create a new NonLinearMemoryPool.
         * \param name Unique pool name.
         * \param bufferUsage Buffer usage flags.
         * \param memoryUsage Memory usage flags.
         * \param blockSize Size of memory blocks in bytes.
         * \param minBlocks Minimum number of memory blocks. If > 0, these blocks are preallocated.
         * \param maxBlocks Maximum number of memory blocks.
         * \return New NonLinearMemoryPool.
         */
        NonLinearMemoryPool& createNonLinearMemoryPool(const std::string&    name,
                                                       VkBufferUsageFlags    bufferUsage,
                                                       VmaMemoryUsage        memoryUsage,
                                                       VkMemoryPropertyFlags requiredMemoryFlags,
                                                       VkMemoryPropertyFlags preferredMemoryFlags,
                                                       size_t                blockSize,
                                                       size_t                minBlocks,
                                                       size_t                maxBlocks);

        /**
         * \brief Create a new RingBufferMemoryPool.
         * \param name Unique pool name.
         * \param bufferUsage Buffer usage flags.
         * \param memoryUsage Memory usage flags.
         * \param blockSize Size of memory block in bytes.
         * \param preallocate Preallocate the memory block.
         * \return New RingBufferMemoryPool.
         */
        RingBufferMemoryPool& createRingBufferMemoryPool(const std::string&    name,
                                                         VkBufferUsageFlags    bufferUsage,
                                                         VmaMemoryUsage        memoryUsage,
                                                         VkMemoryPropertyFlags requiredMemoryFlags,
                                                         VkMemoryPropertyFlags preferredMemoryFlags,
                                                         size_t                blockSize,
                                                         bool                  preallocate);

        /**
         * \brief Create a new StackMemoryPool.
         * \param name Unique pool name.
         * \param bufferUsage Buffer usage flags.
         * \param memoryUsage Memory usage flags.
         * \param blockSize Size of memory blocks in bytes.
         * \param minBlocks Minimum number of memory blocks. If > 0, these blocks are preallocated.
         * \param maxBlocks Maximum number of memory blocks.
         * \return New StackMemoryPool.
         */
        StackMemoryPool& createStackMemoryPool(const std::string&    name,
                                               VkBufferUsageFlags    bufferUsage,
                                               VmaMemoryUsage        memoryUsage,
                                               VkMemoryPropertyFlags requiredMemoryFlags,
                                               VkMemoryPropertyFlags preferredMemoryFlags,
                                               size_t                blockSize,
                                               size_t                minBlocks,
                                               size_t                maxBlocks);

    private:
        void createMemoryPoolImpl(const std::string& name, IMemoryPoolPtr pool);

    public:
        ////////////////////////////////////////////////////////////////
        // Stats.
        ////////////////////////////////////////////////////////////////

        void writeAllocatorStatsToFile(const std::filesystem::path& path) const;

    private:
        ////////////////////////////////////////////////////////////////
        // Initialization.
        ////////////////////////////////////////////////////////////////

        void initializeAllocator();

        void initializeCommandPools();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        VulkanMemoryAllocatorPtr allocator;

        VulkanQueue* computeQueue = nullptr;

        VulkanQueue* graphicsQueue = nullptr;

        VulkanQueue* transferQueue = nullptr;

        std::vector<VulkanCommandPoolPtr> commandPools;

        std::unordered_map<std::string, IMemoryPoolPtr> memoryPools;
    };
}  // namespace sol
