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
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanDevice::VulkanDevice(const Settings& set, const VkDevice vkDevice) : settings(set), device(vkDevice)
    {
        // Get queues.
        queues.reserve(std::accumulate(settings.queues.cbegin(), settings.queues.cend(), static_cast<uint32_t>(0)));
        for (uint32_t i = 0; i < settings.queues.size(); i++)
        {
            auto&   queueFamily = settings.physicalDevice().getQueueFamilies()[i];
            VkQueue q;
            for (uint32_t j = 0; j < settings.queues[i]; j++)
            {
                vkGetDeviceQueue(device, i, j, &q);
                queues.emplace_back(std::make_unique<VulkanQueue>(*this, queueFamily, q));
            }
        }

        // Make queues thread safe if requested.
        if (settings.threadSafeQueues)
            for (const auto& queue : queues) queue->setThreadSafe(true);

        loadExtensionFunctions(set);
    }
#else
    VulkanDevice::VulkanDevice(const Settings& set, const VkDevice vkDevice) :
        physicalDevice(&set.physicalDevice()), device(vkDevice)
    {
        // Get queues.
        queues.reserve(std::accumulate(set.queues.cbegin(), set.queues.cend(), static_cast<uint32_t>(0)));
        for (uint32_t i = 0; i < set.queues.size(); i++)
        {
            auto&   queueFamily = set.physicalDevice().getQueueFamilies()[i];
            VkQueue q;
            for (uint32_t j = 0; j < set.queues[i]; j++)
            {
                vkGetDeviceQueue(device, i, j, &q);
                queues.emplace_back(std::make_unique<VulkanQueue>(*this, queueFamily, q));
            }
        }

        // Make queues thread safe if requested.
        if (set.threadSafeQueues)
            for (const auto& queue : queues) queue->setThreadSafe(true);

        loadExtensionFunctions(set);
    }
#endif

    VulkanDevice::~VulkanDevice() noexcept { vkDestroyDevice(device, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanDevicePtr VulkanDevice::create(const Settings& settings)
    {
        const auto device = createImpl(settings);
        return std::make_unique<VulkanDevice>(settings, device);
    }

    VulkanDeviceSharedPtr VulkanDevice::createShared(const Settings& settings)
    {
        const auto device = createImpl(settings);
        return std::make_shared<VulkanDevice>(settings, device);
    }

    bool VulkanDevice::Settings::validate() const noexcept
    {
        if (!physicalDevice) return false;

        return true;
    }

    VkDevice VulkanDevice::createImpl(const Settings& settings)
    {
        if (!settings.validate()) throw SettingsValidationError("Could not create VulkanDevice. Settings not valid.");

        std::vector priorities(std::ranges::fold_left(settings.queues, 0, std::plus()), 1.0f);
        size_t      offset = 0;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t i = 0; i < settings.queues.size(); i++)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pNext            = nullptr;
            queueCreateInfo.flags            = 0;
            queueCreateInfo.queueFamilyIndex = i;
            queueCreateInfo.queueCount       = settings.queues[i];
            queueCreateInfo.pQueuePriorities = priorities.data() + offset;
            queueCreateInfos.push_back(queueCreateInfo);

            offset += settings.queues[i];
        }

        // Create device info.
        VkDeviceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext                   = &settings.features->features;
        createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos       = queueCreateInfos.data();
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

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanDevice::Settings& VulkanDevice::getSettings() const noexcept { return settings; }
#endif

    VulkanPhysicalDevice& VulkanDevice::getPhysicalDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.physicalDevice();
#else
        return *physicalDevice;
#endif
    }

    const VulkanPhysicalDevice& VulkanDevice::getPhysicalDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.physicalDevice();
#else
        return *physicalDevice;
#endif
    }

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

    ////////////////////////////////////////////////////////////////
    // Extension functions.
    ////////////////////////////////////////////////////////////////

    void VulkanDevice::loadExtensionFunctions(const Settings& set)
    {
        const auto load = [this]<typename F>(F& f, const std::string& name) {
            f = reinterpret_cast<F>(vkGetDeviceProcAddr(device, name.c_str()));
        };

        for (const auto& ext : set.extensions)
        {
            if (ext == VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME)
            {
                load(vkCmdSetPolygonModeEXT, "vkCmdSetPolygonModeEXT");
            }
            else if (ext == VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME)
            {
                load(vkCmdBindDescriptorBufferEmbeddedSamplersEXT, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT");
                load(vkCmdBindDescriptorBuffersEXT, "vkCmdBindDescriptorBuffersEXT");
                load(vkCmdSetDescriptorBufferOffsetsEXT, "vkCmdSetDescriptorBufferOffsetsEXT");
                load(vkGetBufferOpaqueCaptureDescriptorDataEXT, "vkGetBufferOpaqueCaptureDescriptorDataEXT");
                load(vkGetDescriptorEXT, "vkGetDescriptorEXT");
                load(vkGetDescriptorSetLayoutBindingOffsetEXT, "vkGetDescriptorSetLayoutBindingOffsetEXT");
                load(vkGetDescriptorSetLayoutSizeEXT, "vkGetDescriptorSetLayoutSizeEXT");
                load(vkGetImageOpaqueCaptureDescriptorDataEXT, "vkGetImageOpaqueCaptureDescriptorDataEXT");
                load(vkGetImageViewOpaqueCaptureDescriptorDataEXT, "vkGetImageViewOpaqueCaptureDescriptorDataEXT");
                load(vkGetSamplerOpaqueCaptureDescriptorDataEXT, "vkGetSamplerOpaqueCaptureDescriptorDataEXT");
            }
            else if (ext == VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)
            {
                load(vkCmdBuildAccelerationStructuresKHR, "vkCmdBuildAccelerationStructuresKHR");
                load(vkCreateAccelerationStructureKHR, "vkCreateAccelerationStructureKHR");
                load(vkDestroyAccelerationStructureKHR, "vkDestroyAccelerationStructureKHR");
                load(vkGetAccelerationStructureBuildSizesKHR, "vkGetAccelerationStructureBuildSizesKHR");
                load(vkGetAccelerationStructureDeviceAddressKHR, "vkGetAccelerationStructureDeviceAddressKHR");
            }
            else if (ext == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)
            {
                load(vkCreateRayTracingPipelinesKHR, "vkCreateRayTracingPipelinesKHR");
                load(vkGetRayTracingShaderGroupHandlesKHR, "vkGetRayTracingShaderGroupHandlesKHR");
                load(vkCmdTraceRaysKHR, "vkCmdTraceRaysKHR");
            }
        }
    }
}  // namespace sol
