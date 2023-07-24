#include "sol-core/vulkan_top_level_acceleration_structure.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_bottom_level_acceleration_structure.h"
#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure(const Settings&                  set,
                                                                             const VkAccelerationStructureKHR as,
                                                                             VulkanBufferPtr                  b) :
        settings(set), accelerationStructure(as), buffer(std::move(b))
    {
        initDeviceAddress();
    }
#else
    VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure(const Settings&                  set,
                                                                             const VkAccelerationStructureKHR as,
                                                                             VulkanBufferPtr                  b) :
        accelerationStructure(as), buffer(std::move(b))
    {
        initDeviceAddress();
    }
#endif

    VulkanTopLevelAccelerationStructure::~VulkanTopLevelAccelerationStructure() noexcept
    {
        if (accelerationStructure != VK_NULL_HANDLE)
            getDevice().vkDestroyAccelerationStructureKHR(getDevice().get(), accelerationStructure, nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanTopLevelAccelerationStructurePtr VulkanTopLevelAccelerationStructure::create(const Settings& settings)
    {
        auto [as, buffer] = createImpl(settings);
        return std::make_unique<VulkanTopLevelAccelerationStructure>(settings, as, std::move(buffer));
    }

    VulkanTopLevelAccelerationStructureSharedPtr
      VulkanTopLevelAccelerationStructure::createShared(const Settings& settings)
    {
        auto [as, buffer] = createImpl(settings);
        return std::make_shared<VulkanTopLevelAccelerationStructure>(settings, as, std::move(buffer));
    }

    std::pair<VkAccelerationStructureKHR, VulkanBufferPtr>
      VulkanTopLevelAccelerationStructure::createImpl(const Settings& settings)
    {
        auto& device = settings.allocator().getDevice();

        VkTransformMatrixKHR transformMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};

        VkAccelerationStructureInstanceKHR instance{};
        instance.transform                              = transformMatrix;
        instance.instanceCustomIndex                    = 0;
        instance.mask                                   = 0xFF;
        instance.instanceShaderBindingTableRecordOffset = 0;
        instance.flags                                  = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        instance.accelerationStructureReference         = settings.bottomLevelAS().getDeviceAddress();

        // Create instance buffer.
        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device      = device;
        bufferSettings.size        = sizeof(VkAccelerationStructureInstanceKHR);
        bufferSettings.bufferUsage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
                                     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        bufferSettings.allocator         = settings.allocator;
        bufferSettings.vma.memoryUsage   = VMA_MEMORY_USAGE_AUTO;
        bufferSettings.vma.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        bufferSettings.vma.flags =
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        auto instanceBuffer = VulkanBuffer::create(bufferSettings);
        instanceBuffer->setData(&instance, sizeof(VkAccelerationStructureInstanceKHR));

        // Prepare geometry.
        VkAccelerationStructureGeometryKHR geometry{};
        geometry.sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.geometryType             = VK_GEOMETRY_TYPE_INSTANCES_KHR;
        geometry.flags                    = VK_GEOMETRY_OPAQUE_BIT_KHR;
        geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        geometry.geometry.instances.arrayOfPointers    = VK_FALSE;
        geometry.geometry.instances.data.deviceAddress = instanceBuffer->getDeviceAddress();

        // Get size info.
        uint32_t                                 primCount = 1;
        VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        {
            VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
            buildInfo.sType         = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
            buildInfo.type          = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
            buildInfo.flags         = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
            buildInfo.geometryCount = 1;
            buildInfo.pGeometries   = &geometry;
            device.vkGetAccelerationStructureBuildSizesKHR(
              device.get(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, &primCount, &sizeInfo);
        }

        // Create buffer.
        bufferSettings.size = sizeInfo.accelerationStructureSize;
        bufferSettings.bufferUsage =
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        bufferSettings.vma.memoryUsage = VMA_MEMORY_USAGE_AUTO;
        auto buffer                    = VulkanBuffer::create(bufferSettings);

        // Create acceleration structure.
        VkAccelerationStructureKHR           as;
        VkAccelerationStructureCreateInfoKHR asInfo{};
        asInfo.sType  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        asInfo.buffer = buffer->get();
        asInfo.size   = sizeInfo.accelerationStructureSize;
        asInfo.type   = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        device.vkCreateAccelerationStructureKHR(device.get(), &asInfo, nullptr, &as);

        // Create scratch buffer.
        bufferSettings.size        = sizeInfo.buildScratchSize;
        bufferSettings.bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        bufferSettings.vma.alignment =
          256;  // TODO: Hardcoded to 256 because VMA doesn't return properly aligned memory?
        auto scratchBuffer = VulkanBuffer::create(bufferSettings);

        VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo{};
        buildRangeInfo.primitiveCount  = 1;
        buildRangeInfo.primitiveOffset = 0;
        buildRangeInfo.firstVertex     = 0;
        buildRangeInfo.transformOffset = 0;
        std::vector buildRangeInfos    = {&buildRangeInfo};

        // Build acceleration structure.
        VulkanCommandBuffer& commandBuffer = settings.commandBuffer();
        commandBuffer.beginOneTimeCommand();
        {
            VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
            buildInfo.sType                     = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
            buildInfo.type                      = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
            buildInfo.flags                     = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
            buildInfo.mode                      = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
            buildInfo.dstAccelerationStructure  = as;
            buildInfo.geometryCount             = 1;
            buildInfo.pGeometries               = &geometry;
            buildInfo.scratchData.deviceAddress = scratchBuffer->getDeviceAddress();
            device.vkCmdBuildAccelerationStructuresKHR(commandBuffer.get(), 1, &buildInfo, buildRangeInfos.data());
        }
        commandBuffer.endCommand();
        settings.queue().submit(commandBuffer);
        settings.queue().waitIdle();

        return {as, std::move(buffer)};
    }

    void VulkanTopLevelAccelerationStructure::initDeviceAddress()
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
    const VulkanTopLevelAccelerationStructure::Settings&
      VulkanTopLevelAccelerationStructure::getSettings() const noexcept
    {
        return settings;
    }
#endif

    VulkanDevice& VulkanTopLevelAccelerationStructure::getDevice() noexcept { return buffer->getDevice(); }

    const VulkanDevice& VulkanTopLevelAccelerationStructure::getDevice() const noexcept { return buffer->getDevice(); }

    const VkAccelerationStructureKHR& VulkanTopLevelAccelerationStructure::get() const noexcept
    {
        return accelerationStructure;
    }
}  // namespace sol
