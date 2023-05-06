#include "sol-core/vulkan_device.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <numeric>
#include <ranges>

////////////////////////////////////////////////////////////////
// Modue includes.
////////////////////////////////////////////////////////////////

#include "sol-error/settings_validation_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"

namespace sol
{
    VulkanDevice::VulkanDevice(SettingsPtr settingsPtr, const VkDevice vkDevice) :
        settings(std::move(settingsPtr)), device(vkDevice)
    {
        // Get queues.
        queues.reserve(std::accumulate(settings->queues.cbegin(), settings->queues.cend(), static_cast<uint32_t>(0)));
        for (uint32_t i = 0; i < settings->queues.size(); i++)
        {
            auto&   queueFamily = settings->physicalDevice().getQueueFamilies()[i];
            VkQueue q;
            for (uint32_t j = 0; j < settings->queues[i]; j++)
            {
                vkGetDeviceQueue(device, i, j, &q);
                queues.emplace_back(std::make_unique<VulkanQueue>(*this, queueFamily, q));
            }
        }

        // Make queues thread safe if requested.
        if (settings->threadSafeQueues)
            for (const auto& queue : queues) queue->setThreadSafe(true);
    }

    VulkanDevice::~VulkanDevice() noexcept { vkDestroyDevice(device, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDevicePtr VulkanDevice::create(Settings settings)
    {
        const auto device = createImpl(settings);
        return std::make_unique<VulkanDevice>(std::make_unique<Settings>(std::move(settings)), device);
    }

    VulkanDeviceSharedPtr VulkanDevice::createShared(Settings settings)
    {
        const auto device = createImpl(settings);
        return std::make_shared<VulkanDevice>(std::make_unique<Settings>(std::move(settings)), device);
    }

    bool VulkanDevice::Settings::validate() const noexcept
    {
        if (!physicalDevice) return false;

        return true;
    }

    VkDevice VulkanDevice::createImpl(const Settings& settings)
    {
        if (!settings.validate()) throw SettingsValidationError("Could not create VulkanDevice. Settings not valid.");

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t i = 0; i < settings.queues.size(); i++)
        {
            std::vector             priorities(settings.queues[i], 1.0f);
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pNext            = nullptr;
            queueCreateInfo.flags            = 0;
            queueCreateInfo.queueFamilyIndex = i;
            queueCreateInfo.queueCount       = settings.queues[i];
            queueCreateInfo.pQueuePriorities = priorities.data();
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // TODO: Expose these properties.
        // Create device features.
        VkPhysicalDeviceFeatures deviceFeatures{};

        // Create device info.
        VkDeviceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos       = queueCreateInfos.data();
        createInfo.pEnabledFeatures        = &deviceFeatures;
        const auto cstrings                = stringVectorToConstCharVector(settings.extensions);
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(cstrings.size());
        createInfo.ppEnabledExtensionNames = cstrings.data();

        // Create device.
        VkDevice device;
        handleVulkanError(vkCreateDevice(settings.physicalDevice, &createInfo, nullptr, &device));

        return device;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanDevice::Settings& VulkanDevice::getSettings() const noexcept { return *settings; }

    VulkanPhysicalDevice& VulkanDevice::getPhysicalDevice() noexcept { return settings->physicalDevice(); }

    const VulkanPhysicalDevice& VulkanDevice::getPhysicalDevice() const noexcept { return settings->physicalDevice(); }

    const VkDevice& VulkanDevice::get() const noexcept { return device; }

    const std::vector<VulkanQueuePtr>& VulkanDevice::getQueues() const noexcept { return queues; }

    std::vector<VulkanQueue*> VulkanDevice::getQueues(const VulkanQueueFamily& family) const
    {
        std::vector<VulkanQueue*> familyQueues;

        for (auto& q : queues | std::views::filter(
                                  [&family](const VulkanQueuePtr& queue) { return &queue->getFamily() == &family; }))
            familyQueues.emplace_back(q.get());

        return familyQueues;
    }
}  // namespace sol
