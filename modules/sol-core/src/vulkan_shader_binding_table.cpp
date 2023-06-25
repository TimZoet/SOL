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
    VulkanShaderBindingTable::VulkanShaderBindingTable(const Settings&                       set,
                                                       VulkanBufferPtr                       sbtBuffer,
                                                       const VkStridedDeviceAddressRegionKHR raygen,
                                                       const VkStridedDeviceAddressRegionKHR miss,
                                                       const VkStridedDeviceAddressRegionKHR hit,
                                                       const VkStridedDeviceAddressRegionKHR callable) :
        settings(set),
        buffer(std::move(sbtBuffer)),
        raygenRegion(raygen),
        missRegion(miss),
        hitRegion(hit),
        callableRegion(callable)
    {
    }
#else
    VulkanShaderBindingTable::VulkanShaderBindingTable(const Settings&                       set,
                                                       VulkanBufferPtr                       sbtBuffer,
                                                       const VkStridedDeviceAddressRegionKHR raygen,
                                                       const VkStridedDeviceAddressRegionKHR miss,
                                                       const VkStridedDeviceAddressRegionKHR hit,
                                                       const VkStridedDeviceAddressRegionKHR callable) :
        pipeline(&set.pipeline()),
        buffer(std::move(sbtBuffer)),
        raygenRegion(raygen),
        missRegion(miss),
        hitRegion(hit),
        callableRegion(callable)
    {
    }
#endif

    VulkanShaderBindingTable::~VulkanShaderBindingTable() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanShaderBindingTablePtr VulkanShaderBindingTable::create(const Settings& settings)
    {
        auto [b, r0, r1, r2, r3] = createImpl(settings);
        return std::make_unique<VulkanShaderBindingTable>(settings, std::move(b), r0, r1, r2, r3);
    }

    VulkanShaderBindingTableSharedPtr VulkanShaderBindingTable::createShared(const Settings& settings)
    {
        auto [b, r0, r1, r2, r3] = createImpl(settings);
        return std::make_shared<VulkanShaderBindingTable>(settings, std::move(b), r0, r1, r2, r3);
    }

    std::tuple<VulkanBufferPtr,
               VkStridedDeviceAddressRegionKHR,
               VkStridedDeviceAddressRegionKHR,
               VkStridedDeviceAddressRegionKHR,
               VkStridedDeviceAddressRegionKHR>
      VulkanShaderBindingTable::createImpl(const Settings& settings)
    {
        constexpr auto alignUp = [](auto v, auto a) { return v + (a - 1) & ~(a - 1); };

        // TODO: Perhaps get these properties from elsewhere instead of getting them here again and again?
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProps{
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};
        VkPhysicalDeviceProperties2 deviceProperties2{};
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.pNext = &rtProps;
        vkGetPhysicalDeviceProperties2(settings.device().getPhysicalDevice().get(), &deviceProperties2);

        const auto&    pipeline          = settings.pipeline();
        const auto     raygenCount       = pipeline.getRaygenGroupCount();
        const auto     missCount         = pipeline.getMissGroupCount();
        const auto     hitCount          = pipeline.getHitGroupCount();
        const auto     callableCount     = pipeline.getCallableGroupCount();
        const auto     handleCount       = raygenCount + missCount + hitCount + callableCount;
        const auto     handleSize        = rtProps.shaderGroupHandleSize;
        const uint32_t handleSizeAligned = alignUp(handleSize, rtProps.shaderGroupHandleAlignment);

        // Retrieve handles.
        std::vector<uint8_t> handles(static_cast<size_t>(handleCount) * handleSize);
        handleVulkanError(settings.device().vkGetRayTracingShaderGroupHandlesKHR(
          settings.device, settings.pipeline, 0, handleCount, handles.size(), handles.data()));

        // Prepare strides and sizes of device address regions.
        VkStridedDeviceAddressRegionKHR raygenRegion{};
        VkStridedDeviceAddressRegionKHR missRegion{};
        VkStridedDeviceAddressRegionKHR hitRegion{};
        VkStridedDeviceAddressRegionKHR callableRegion{};
        raygenRegion.stride   = alignUp(handleSizeAligned, rtProps.shaderGroupBaseAlignment);
        raygenRegion.size     = raygenRegion.stride;
        missRegion.stride     = handleSizeAligned;
        missRegion.size       = alignUp(missCount * handleSizeAligned, rtProps.shaderGroupBaseAlignment);
        hitRegion.stride      = handleSizeAligned;
        hitRegion.size        = alignUp(hitCount * handleSizeAligned, rtProps.shaderGroupBaseAlignment);
        callableRegion.stride = handleSizeAligned;
        callableRegion.size   = alignUp(callableCount * handleSizeAligned, rtProps.shaderGroupBaseAlignment);

        // Allocate shader binding table buffer.
        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device = settings.device;
        bufferSettings.size   = raygenRegion.size + missRegion.size + hitRegion.size + callableRegion.size;
        bufferSettings.bufferUsage =
          VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        bufferSettings.allocator     = settings.allocator;
        bufferSettings.memoryUsage   = VMA_MEMORY_USAGE_AUTO;
        bufferSettings.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        bufferSettings.flags =
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        auto b = VulkanBuffer::create(bufferSettings);

        // Calculate device addresses.
        raygenRegion.deviceAddress = b->getDeviceAddress();
        missRegion.deviceAddress   = raygenRegion.deviceAddress + raygenRegion.size;
        if (hitCount) hitRegion.deviceAddress = missRegion.deviceAddress + missRegion.size;
        if (callableCount) callableRegion.deviceAddress = hitRegion.deviceAddress + hitRegion.size;

        // Copy raygen handle.
        size_t handleOffset = 0, sbtOffset = 0;
        b->setData(handles.data() + handleOffset, handleSize, sbtOffset);

        // Copy miss handles.
        handleOffset = handleSize;
        sbtOffset    = raygenRegion.size;
        for (uint32_t i = 0; i < missCount; i++)
        {
            b->setData(handles.data() + handleOffset, handleSize, sbtOffset);
            handleOffset += handleSize;
            sbtOffset += missRegion.stride;
        }

        // Copy hit handles.
        handleOffset = static_cast<size_t>(handleSize) * (1 + missCount);
        sbtOffset    = raygenRegion.size + missRegion.size;
        for (uint32_t i = 0; i < hitCount; i++)
        {
            b->setData(handles.data() + handleOffset, handleSize, sbtOffset);
            handleOffset += handleSize;
            sbtOffset += hitRegion.stride;
        }

        // Copy callable handles.
        handleOffset = static_cast<size_t>(handleSize) * (1 + missCount + hitCount);
        sbtOffset    = raygenRegion.size + missRegion.size + hitRegion.size;
        for (uint32_t i = 0; i < callableCount; i++)
        {
            b->setData(handles.data() + handleOffset, handleSize, sbtOffset);
            handleOffset += handleSize;
            sbtOffset += callableRegion.stride;
        }

        return {std::move(b), raygenRegion, missRegion, hitRegion, callableRegion};
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
        return pipeline->getDevice();
#endif
    }

    const VulkanDevice& VulkanShaderBindingTable::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return pipeline->getDevice();
#endif
    }

    VkStridedDeviceAddressRegionKHR VulkanShaderBindingTable::getRaygenRegion() const noexcept { return raygenRegion; }

    VkStridedDeviceAddressRegionKHR VulkanShaderBindingTable::getMissRegion() const noexcept { return missRegion; }

    VkStridedDeviceAddressRegionKHR VulkanShaderBindingTable::getHitRegion() const noexcept { return hitRegion; }

    VkStridedDeviceAddressRegionKHR VulkanShaderBindingTable::getCallableRegion() const noexcept
    {
        return callableRegion;
    }
}  // namespace sol
