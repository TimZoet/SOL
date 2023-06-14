#include "sol-render/common/uniform_buffer_manager.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <numeric>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_descriptor_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    UniformBufferManager::UniformBufferManager(MemoryManager& memManager, const size_t dataSetCount) :
        memoryManager(&memManager), setCount(dataSetCount)
    {
        assert(setCount > 0);
    }

    UniformBufferManager::~UniformBufferManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MemoryManager& UniformBufferManager::getMemoryManager() noexcept { return *memoryManager; }

    const MemoryManager& UniformBufferManager::getMemoryManager() const noexcept { return *memoryManager; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    UniformBuffer* UniformBufferManager::create(const Material&                                material,
                                                const uint32_t                                 setIndex,
                                                const uint32_t                                 bindingIndex,
                                                const MaterialLayoutDescription::SharingMethod sharingMethod,
                                                const size_t                                   slotCount,
                                                const size_t                                   bufferSize)
    {
        // Allocate buffers.
        std::vector<VulkanBufferPtr> buffers(setCount);
        VulkanBuffer::Settings       bufferSettings;
        bufferSettings.device      = memoryManager->getDevice();
        bufferSettings.size        = slotCount * bufferSize;
        bufferSettings.bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferSettings.allocator   = memoryManager->getAllocator();
        bufferSettings.memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        for (size_t i = 0; i < setCount; i++) buffers[i] = VulkanBuffer::create(bufferSettings);

        // Create new UniformBuffer.
        auto uniformBuffer = std::make_unique<UniformBuffer>(
          sharingMethod, setIndex, bindingIndex, slotCount, bufferSize, std::move(buffers));
        auto* ub = uniformBuffer.get();

        if (const auto it = bufferMap.find(&material); it == bufferMap.end())
        {
            std::vector<UniformBufferPtr> vec;
            vec.emplace_back(std::move(uniformBuffer));
            bufferMap.try_emplace(&material, std::move(vec));
        }
        else
            it->second.emplace_back(std::move(uniformBuffer));

        return ub;
    }

    UniformBuffer* UniformBufferManager::getOrCreate(const Material&                                material,
                                                     const uint32_t                                 setIndex,
                                                     const uint32_t                                 bindingIndex,
                                                     const MaterialLayoutDescription::SharingMethod sharingMethod,
                                                     const size_t                                   slotCount,
                                                     const size_t                                   bufferSize)
    {
        const auto it = bufferMap.find(&material);
        if (it == bufferMap.end())
            return create(material, setIndex, bindingIndex, sharingMethod, slotCount, bufferSize);

        const auto& buffers = it->second;
        const auto  it2     = std::ranges::find_if(buffers.begin(), buffers.end(), [&](const UniformBufferPtr& elem) {
            return elem->getSet() == setIndex && elem->getBinding() == bindingIndex &&
                   elem->getSharingMethod() == sharingMethod && !elem->isFull();
        });

        if (it2 == buffers.end()) return create(material, setIndex, bindingIndex, sharingMethod, slotCount, bufferSize);
        return it2->get();
    }

    void UniformBufferManager::mapAll(const uint32_t setIndex)
    {
        for (const auto& list : bufferMap | std::views::values)
            for (auto& buffer : list) buffer->map(setIndex);
    }

    void UniformBufferManager::unmapAll(const uint32_t setIndex)
    {
        for (const auto& list : bufferMap | std::views::values)
            for (auto& buffer : list) buffer->unmap(setIndex);
    }

    void UniformBufferManager::flushAll(const uint32_t setIndex)
    {
        for (const auto& list : bufferMap | std::views::values)
            for (auto& buffer : list) buffer->flush(setIndex);
    }
}  // namespace sol
