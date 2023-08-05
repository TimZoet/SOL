#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/vulkan_memory_pool.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pool/i_memory_pool.h"
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

        [[nodiscard]] IMemoryPool& getMemoryPool(const std::string& name);

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
         * \param info Creation parameters.
         * \param args Additional parameter values passed to the constructor.
         * \return New memory pool.
         */
        template<std::derived_from<IMemoryPool> Pool, typename... Args>
            requires std::constructible_from<Pool,
                                             MemoryManager&,
                                             std::string,
                                             const IMemoryPool::CreateInfo&,
                                             VulkanMemoryPoolPtr,
                                             Args...>
        Pool& createMemoryPool(const std::string& name, const IMemoryPool::CreateInfo& info, Args&&... args)
        {
            auto vkPool = IMemoryPool::create(*allocator, info);
            auto pool =
              std::make_unique<Pool>(*this, name, info, std::move(vkPool), std::forward<Args>(args)...);
            auto& poolRef = *pool;
            if (const auto [_, inserted] = memoryPools.try_emplace(name, std::move(pool)); !inserted)
                throw SolError(std::format("Cannot create new memory pool. Name {} is already in use.", name));

            return poolRef;
        }

        /**
         * \brief Create a new FreeAtOnceMemoryPool.
         * \param name Unique pool name.
         * \param createInfo Creation parameters.
         * \return New FreeAtOnceMemoryPool.
         */
        FreeAtOnceMemoryPool& createFreeAtOnceMemoryPool(const std::string& name, IMemoryPool::CreateInfo createInfo);

        /**
         * \brief Create a new NonLinearMemoryPool.
         * \param name Unique pool name.
         * \param createInfo Creation parameters.
         * \return New NonLinearMemoryPool.
         */
        NonLinearMemoryPool& createNonLinearMemoryPool(const std::string& name, IMemoryPool::CreateInfo createInfo);

        /**
         * \brief Create a new RingBufferMemoryPool.
         * \param name Unique pool name.
         * \param createInfo Creation parameters.
         * \return New RingBufferMemoryPool.
         */
        RingBufferMemoryPool& createRingBufferMemoryPool(const std::string& name, IMemoryPool::CreateInfo createInfo);

        /**
         * \brief Create a new StackMemoryPool.
         * \param name Unique pool name.
         * \param createInfo Creation parameters.
         * \return New StackMemoryPool.
         */
        StackMemoryPool& createStackMemoryPool(const std::string& name, IMemoryPool::CreateInfo createInfo);

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
