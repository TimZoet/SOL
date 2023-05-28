#include "sol-core/vulkan_shader_binding_table.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_ray_tracing_pipeline.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanShaderBindingTable::VulkanShaderBindingTable(const Settings& set, std::vector<VulkanBufferPtr> b) :
        settings(set), buffers(std::move(b))
    {
        // TODO: Perhaps get these properties from elsewhere instead of getting them here again and again?
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};
        rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &rayTracingPipelineProperties;
        vkGetPhysicalDeviceProperties2(getDevice().getPhysicalDevice().get(), &deviceProperties2);

        const uint32_t handleSize        = rayTracingPipelineProperties.shaderGroupHandleSize;
        const uint32_t alignment         = rayTracingPipelineProperties.shaderGroupHandleAlignment;
        const uint32_t handleSizeAligned = (handleSize + alignment - 1) & ~(alignment - 1);
        for (const auto& buf : buffers)
        {
            regions.emplace_back(buf->getDeviceAddress(), handleSizeAligned, handleSizeAligned);
        }
    }
#else
    VulkanShaderBindingTable::VulkanShaderBindingTable(const Settings& set, std::vector<VulkanBufferPtr> b) :
        settings(), buffers(std::move(b))
    {
        // TODO: Perhaps get these properties from elsewhere instead of getting them here again and again?
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};
        rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &rayTracingPipelineProperties;
        vkGetPhysicalDeviceProperties2(getDevice().getPhysicalDevice().get(), &deviceProperties2);

        const uint32_t handleSize        = rayTracingPipelineProperties.shaderGroupHandleSize;
        const uint32_t alignment         = rayTracingPipelineProperties.shaderGroupHandleAlignment;
        const uint32_t handleSizeAligned = (handleSize + alignment - 1) & ~(alignment - 1);
        for (const auto& buf : buffers)
        {
            regions.emplace_back(buf->getDeviceAddress(), handleSizeAligned, handleSizeAligned);
        }
    }
#endif

    VulkanShaderBindingTable::~VulkanShaderBindingTable() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanShaderBindingTablePtr VulkanShaderBindingTable::create(const Settings& settings)
    {
        return std::make_unique<VulkanShaderBindingTable>(settings, createImpl(settings));
    }

    VulkanShaderBindingTableSharedPtr VulkanShaderBindingTable::createShared(const Settings& settings)
    {
        return std::make_shared<VulkanShaderBindingTable>(settings, createImpl(settings));
    }

    std::vector<VulkanBufferPtr> VulkanShaderBindingTable::createImpl(const Settings& settings)
    {
        // TODO: Perhaps get these properties from elsewhere instead of getting them here again and again?
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};
        rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
        VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &rayTracingPipelineProperties;
        vkGetPhysicalDeviceProperties2(settings.device().getPhysicalDevice().get(), &deviceProperties2);

        const uint32_t handleSize        = rayTracingPipelineProperties.shaderGroupHandleSize;
        const uint32_t alignment         = rayTracingPipelineProperties.shaderGroupHandleAlignment;
        const uint32_t handleSizeAligned = (handleSize + alignment - 1) & ~(alignment - 1);
        const uint32_t groupCount        = settings.pipeline().getShaderGroupCount();
        const uint32_t sbtSize           = groupCount * handleSizeAligned;

        std::vector<uint8_t> shaderHandleStorage(sbtSize);
        handleVulkanError(settings.device().vkGetRayTracingShaderGroupHandlesKHR(settings.device().get(),
                                                                                 settings.pipeline().getPipeline(),
                                                                                 0,
                                                                                 groupCount,
                                                                                 sbtSize,
                                                                                 shaderHandleStorage.data()));


        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device = settings.device;
        bufferSettings.size   = sbtSize;
        bufferSettings.bufferUsage =
          VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        bufferSettings.allocator     = settings.allocator;
        bufferSettings.memoryUsage   = VMA_MEMORY_USAGE_AUTO;
        bufferSettings.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        bufferSettings.flags =
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        bufferSettings.alignment =
          rayTracingPipelineProperties
            .shaderGroupBaseAlignment;  //TODO: Why do examples specify any alignment explicitly?

        std::vector<VulkanBufferPtr> buffers;
        for (uint32_t i = 0; i < groupCount; i++)
        {
            auto b = VulkanBuffer::create(bufferSettings);
            b->setData(shaderHandleStorage.data() + handleSizeAligned * i, handleSize);
            buffers.emplace_back(std::move(b));
        }

        return buffers;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanShaderBindingTable::Settings& VulkanShaderBindingTable::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanShaderBindingTable::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return buffer->getDevice();
#endif
    }

    const VulkanDevice& VulkanShaderBindingTable::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return buffer->getDevice();
#endif
    }

    VkStridedDeviceAddressRegionKHR VulkanShaderBindingTable::getRegion(const size_t i) const noexcept
    {
        if (i >= regions.size()) return VkStridedDeviceAddressRegionKHR{};
        return regions.at(i);
    }

}  // namespace sol
