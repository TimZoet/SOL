#include "testutils/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transfer_manager.h"

namespace
{
    sol::VulkanInstancePtr                instance;
    sol::VulkanPhysicalDeviceFeatures2Ptr supportedFeatures;
    sol::VulkanPhysicalDeviceFeatures2Ptr enabledFeatures;
    sol::VulkanPhysicalDevicePtr          physicalDevice;
    sol::VulkanDevicePtr                  device;
    sol::MemoryManagerPtr                 memoryManager;
    sol::TransferManagerPtr               transferManager;

    void createDefaultInstance()
    {
        sol::VulkanInstance::Settings settings;
        settings.applicationName    = "SolTest";
        settings.applicationVersion = sol::Version(1, 0, 0);
        instance                    = sol::VulkanInstance::create(settings);
    }

    void createSupportedFeatures()
    {
        supportedFeatures =
          std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                              sol::VulkanPhysicalDeviceVulkan12Features,
                                                              sol::VulkanPhysicalDeviceVulkan13Features>>();
    }

    void createEnabledFeatures()
    {
        enabledFeatures =
          std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                              sol::VulkanPhysicalDeviceVulkan12Features,
                                                              sol::VulkanPhysicalDeviceVulkan13Features>>();

        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing  = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->timelineSemaphore   = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering    = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2    = VK_TRUE;
    }

    void createDefaultPhysicalDevice()
    {
        sol::VulkanPhysicalDevice::Settings settings;
        settings.instance       = instance;
        settings.propertyFilter = [](const VkPhysicalDeviceProperties& props) {
            return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        };
        // TODO: Many of these features are kinda needed by SOL out of the box. Perhaps something needs to be built in to SOL to improve this.
        settings.features      = supportedFeatures.get();
        settings.featureFilter = [](sol::RootVulkanPhysicalDeviceFeatures2& features) {
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->timelineSemaphore) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2) return false;
            return true;
        };
        settings.queueFamilyFilter = [](const std::vector<sol::VulkanQueueFamily>& queues) {
            bool hasGraphics          = false;
            bool hasDedicatedTransfer = false;

            for (const auto& q : queues)
            {
                if (q.supportsGraphics()) hasGraphics = true;
                if (q.supportsDedicatedTransfer()) hasDedicatedTransfer = true;
            }

            return hasGraphics && hasDedicatedTransfer;
        };
        physicalDevice = sol::VulkanPhysicalDevice::create(settings);
    }

    void createDefaultDevice()
    {
        sol::VulkanDevice::Settings settings;
        settings.physicalDevice = physicalDevice;
        settings.extensions     = physicalDevice->getSettings().extensions;
        settings.features       = enabledFeatures.get();
        settings.queues.resize(physicalDevice->getQueueFamilies().size(), 1);
        settings.threadSafeQueues = true;
        device                    = sol::VulkanDevice::create(settings);
    }

    void createDefaultMemoryManager()
    {
        sol::VulkanMemoryAllocator::Settings settings;
        settings.device = *device;
        settings.flags  = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        memoryManager   = std::make_unique<sol::MemoryManager>(sol::VulkanMemoryAllocator::create(settings));
        for (auto& queue : device->getQueues())
        {
            if (queue->getFamily().supportsCompute()) memoryManager->setComputeQueue(*queue);
            if (queue->getFamily().supportsGraphics()) memoryManager->setGraphicsQueue(*queue);
            if (queue->getFamily().supportsDedicatedTransfer()) memoryManager->setTransferQueue(*queue);
        }
    }

    void createDefaultTransferManager()
    {
        constexpr sol::IMemoryPool::CreateInfo info{
          .createFlags          = 0,
          .bufferUsage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
          .blockSize       = 256ull * 1024ull * 1024ull,
          .minBlocks       = 1,
          .maxBlocks       = 1};
        auto& pool      = memoryManager->createRingBufferMemoryPool("transfer", info);
        transferManager = std::make_unique<sol::TransferManager>(*memoryManager, pool);
    }
}  // namespace



BasicFixture::BasicFixture()
{
    createDefaultInstance();
    createSupportedFeatures();
    createEnabledFeatures();
    createDefaultPhysicalDevice();
    createDefaultDevice();
}

BasicFixture::~BasicFixture() noexcept
{
    device.reset();
    physicalDevice.reset();
    enabledFeatures.reset();
    supportedFeatures.reset();
    instance.reset();
}

sol::VulkanInstance& BasicFixture::getInstance() { return *instance; }

sol::VulkanPhysicalDevice& BasicFixture::getPhysicalDevice() { return *physicalDevice; }

sol::VulkanDevice& BasicFixture::getDevice() { return *device; }

MemoryManagerFixture::MemoryManagerFixture()
{
    createDefaultMemoryManager();
    createDefaultTransferManager();
}

MemoryManagerFixture::~MemoryManagerFixture() noexcept
{
    transferManager.reset();
    memoryManager.reset();
}

sol::MemoryManager& MemoryManagerFixture::getMemoryManager() { return *memoryManager; }

sol::TransferManager& MemoryManagerFixture::getTransferManager() { return *transferManager; }

std::vector<uint32_t> ImageDataGeneration::genR8G8B8A8W256H256Gradient()
{
    std::vector<uint32_t> data(256ull * 256ull, 0);
    for (const auto y : std::views::iota(0) | std::views::take(256))
        for (const auto x : std::views::iota(0) | std::views::take(256))
            data[x + y * 256] = x << 24 | y << 16 | std::max(x, y) << 8 | 0xff;

    return data;
}
