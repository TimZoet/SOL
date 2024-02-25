#include "sol-core/vulkan_bottom_level_acceleration_structure.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_device_memory.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanBottomLevelAccelerationStructure::VulkanBottomLevelAccelerationStructure(const Settings&                  set,
                                                                                   const VkAccelerationStructureKHR as,
                                                                                   VulkanBufferPtr                  b) :
        settings(set), accelerationStructure(as), buffer(std::move(b))
    {
        initDeviceAddress();
    }
#else
    VulkanBottomLevelAccelerationStructure::VulkanBottomLevelAccelerationStructure(const Settings&                  set,
                                                                                   const VkAccelerationStructureKHR as,
                                                                                   VulkanBufferPtr                  b) :
        accelerationStructure(as), buffer(std::move(b))
    {
        initDeviceAddress();
    }
#endif

    VulkanBottomLevelAccelerationStructure::~VulkanBottomLevelAccelerationStructure() noexcept
    {
        if (accelerationStructure != VK_NULL_HANDLE)
            getDevice().vkDestroyAccelerationStructureKHR(getDevice().get(), accelerationStructure, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanBottomLevelAccelerationStructurePtr VulkanBottomLevelAccelerationStructure::create(const Settings& settings)
    {
        auto [as, buffer] = createImpl(settings);
        return std::make_unique<VulkanBottomLevelAccelerationStructure>(settings, as, std::move(buffer));
    }

    VulkanBottomLevelAccelerationStructureSharedPtr
      VulkanBottomLevelAccelerationStructure::createShared(const Settings& settings)
    {
        auto [as, buffer] = createImpl(settings);
        return std::make_shared<VulkanBottomLevelAccelerationStructure>(settings, as, std::move(buffer));
    }

    std::pair<VkAccelerationStructureKHR, VulkanBufferPtr>
      VulkanBottomLevelAccelerationStructure::createImpl(const Settings& settings)
    {
        auto& device = settings.allocator().getDevice();

        const VkDeviceOrHostAddressConstKHR vertexAddress{.deviceAddress = settings.vertexBuffer().getDeviceAddress()};
        const VkDeviceOrHostAddressConstKHR indexAddress{.deviceAddress = settings.indexBuffer().getDeviceAddress()};

        // Prepare geometry.
        VkAccelerationStructureGeometryKHR geometry{};
        geometry.sType                           = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.flags                           = VK_GEOMETRY_OPAQUE_BIT_KHR;
        geometry.geometryType                    = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        geometry.geometry.triangles.sType        = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        geometry.geometry.triangles.vertexData   = vertexAddress;
        geometry.geometry.triangles.maxVertex    = settings.maxVertex;
        geometry.geometry.triangles.vertexStride = settings.vertexStride;
        geometry.geometry.triangles.indexType    = VK_INDEX_TYPE_UINT16;
        geometry.geometry.triangles.indexData    = indexAddress;
        geometry.geometry.triangles.transformData.deviceAddress = 0;
        geometry.geometry.triangles.transformData.hostAddress   = nullptr;

        // Prepare build info.
        VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
        buildInfo.sType         = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type          = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        buildInfo.flags         = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        buildInfo.geometryCount = 1;
        buildInfo.pGeometries   = &geometry;

        // Get size info.
        VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        device.vkGetAccelerationStructureBuildSizesKHR(
          device.get(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, &settings.numTriangles, &sizeInfo);

        // Create buffer.
        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device = device;
        bufferSettings.size   = sizeInfo.accelerationStructureSize;
        bufferSettings.bufferUsage =
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        bufferSettings.allocator       = settings.allocator;
        bufferSettings.vma.memoryUsage = VMA_MEMORY_USAGE_AUTO;
        auto buffer                    = VulkanBuffer::create(bufferSettings);

        // Create acceleration structure.
        VkAccelerationStructureKHR           as;
        VkAccelerationStructureCreateInfoKHR asInfo{};
        asInfo.sType  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        asInfo.buffer = buffer->get();
        asInfo.size   = sizeInfo.accelerationStructureSize;
        asInfo.type   = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        device.vkCreateAccelerationStructureKHR(device.get(), &asInfo, nullptr, &as);

        // Create scratch buffer.
        bufferSettings.size            = sizeInfo.buildScratchSize;
        bufferSettings.bufferUsage     = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        bufferSettings.vma.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        bufferSettings.vma.alignment =
          256;  // TODO: Hardcoded to 256 because VMA doesn't return properly aligned memory?
        auto scratchBuffer = VulkanBuffer::create(bufferSettings);

        buildInfo.mode                      = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.dstAccelerationStructure  = as;
        buildInfo.scratchData.deviceAddress = scratchBuffer->getDeviceAddress();

        VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo{};
        buildRangeInfo.primitiveCount  = settings.numTriangles;
        buildRangeInfo.primitiveOffset = 0;
        buildRangeInfo.firstVertex     = 0;
        buildRangeInfo.transformOffset = 0;
        std::vector buildRangeInfos    = {&buildRangeInfo};

        // Build acceleration structure.
        VulkanCommandBuffer& commandBuffer = settings.commandBuffer();
        commandBuffer.beginOneTimeCommand();
        device.vkCmdBuildAccelerationStructuresKHR(commandBuffer.get(), 1, &buildInfo, buildRangeInfos.data());
        commandBuffer.endCommand();
        settings.queue().submit(commandBuffer);
        settings.queue().waitIdle();

        return {as, std::move(buffer)};
    }

    void VulkanBottomLevelAccelerationStructure::initDeviceAddress()
    {
        VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
        accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        accelerationDeviceAddressInfo.accelerationStructure = accelerationStructure;
        deviceAddress =
          getDevice().vkGetAccelerationStructureDeviceAddressKHR(getDevice().get(), &accelerationDeviceAddressInfo);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanBottomLevelAccelerationStructure::Settings&
      VulkanBottomLevelAccelerationStructure::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanBottomLevelAccelerationStructure::getDevice() noexcept { return buffer->getDevice(); }

    const VulkanDevice& VulkanBottomLevelAccelerationStructure::getDevice() const noexcept
    {
        return buffer->getDevice();
    }

    VkDeviceAddress VulkanBottomLevelAccelerationStructure::getDeviceAddress() const noexcept { return deviceAddress; }
}  // namespace sol
