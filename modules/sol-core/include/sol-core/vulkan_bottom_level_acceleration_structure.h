#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <utility>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

namespace sol
{
    class VulkanBottomLevelAccelerationStructure
    {
    public:
        /**
         * \brief VulkanBottomLevelAccelerationStructure settings.
         */
        struct Settings
        {
            ObjectRefSetting<VulkanCommandBuffer> commandBuffer;

            ObjectRefSetting<VulkanQueue> queue;

            ObjectRefSetting<VulkanMemoryAllocator> allocator;

            ObjectRefSetting<VulkanBuffer> vertexBuffer;

            ObjectRefSetting<VulkanBuffer> indexBuffer;

            uint32_t numTriangles = 0;

            uint32_t maxVertex = 0;

            VkDeviceSize vertexStride = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanBottomLevelAccelerationStructure() = delete;

        VulkanBottomLevelAccelerationStructure(const Settings& set, VkAccelerationStructureKHR as, VulkanBufferPtr b);

        VulkanBottomLevelAccelerationStructure(const VulkanBottomLevelAccelerationStructure&) = delete;

        VulkanBottomLevelAccelerationStructure(VulkanBottomLevelAccelerationStructure&&) = delete;

        ~VulkanBottomLevelAccelerationStructure() noexcept;

        VulkanBottomLevelAccelerationStructure& operator=(const VulkanBottomLevelAccelerationStructure&) = delete;

        VulkanBottomLevelAccelerationStructure& operator=(VulkanBottomLevelAccelerationStructure&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan bottom level acceleration structure.
         * \param settings Settings.
         * \throws VulkanError Thrown if creation failed.
         * \return Vulkan bottom level acceleration structure.
         */
        [[nodiscard]] static VulkanBottomLevelAccelerationStructurePtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan bottom level acceleration structure.
         * \param settings Settings.
         * \throws VulkanError Thrown if creation failed.
         * \return Vulkan bottom level acceleration structure.
         */
        [[nodiscard]] static VulkanBottomLevelAccelerationStructureSharedPtr createShared(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;
#endif

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        /**
         * \brief Get the device address.
         * \return VkDeviceAddress.
         */
        [[nodiscard]] VkDeviceAddress getDeviceAddress() const noexcept;

    private:
        [[nodiscard]] static std::pair<VkAccelerationStructureKHR, VulkanBufferPtr>
          createImpl(const Settings& settings);

        void initDeviceAddress();

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#endif

        /**
         * \brief Vulkan acceleration structure.
         */
        VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;

        VkDeviceAddress deviceAddress = 0;

        /**
         * \brief Vulkan buffer holding acceleration structure data.
         */
        VulkanBufferPtr buffer;
    };
}  // namespace sol
