#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
#include <utility>
#include <vector>

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
    class VulkanTopLevelAccelerationStructure
    {
    public:
        /**
         * \brief VulkanTopLevelAccelerationStructure settings.
         */
        struct Settings
        {
            ObjectRefSetting<VulkanMemoryAllocator> allocator;

            ObjectRefSetting<VulkanCommandBuffer> commandBuffer;

            ObjectRefSetting<VulkanQueue> queue;

            ObjectRefSetting<VulkanBottomLevelAccelerationStructure> bottomLevelAS;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanTopLevelAccelerationStructure() = delete;

        VulkanTopLevelAccelerationStructure(const Settings& set, VkAccelerationStructureKHR as, VulkanBufferPtr b);

        VulkanTopLevelAccelerationStructure(const VulkanTopLevelAccelerationStructure&) = delete;

        VulkanTopLevelAccelerationStructure(VulkanTopLevelAccelerationStructure&&) = delete;

        ~VulkanTopLevelAccelerationStructure() noexcept;

        VulkanTopLevelAccelerationStructure& operator=(const VulkanTopLevelAccelerationStructure&) = delete;

        VulkanTopLevelAccelerationStructure& operator=(VulkanTopLevelAccelerationStructure&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create / Destroy.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan top level acceleration structure.
         * \param settings Settings.
         * \throws VulkanError Thrown if creation failed.
         * \return Vulkan top level acceleration structure.
         */
        [[nodiscard]] static VulkanTopLevelAccelerationStructurePtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan top level acceleration structure.
         * \param settings Settings.
         * \throws VulkanError Thrown if creation failed.
         * \return Vulkan top level acceleration structure.
         */
        [[nodiscard]] static VulkanTopLevelAccelerationStructureSharedPtr createShared(const Settings& settings);

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
         * \brief Get the acceleration structure managed by this object.
         * \return Acceleration structure handle.
         */
        [[nodiscard]] const VkAccelerationStructureKHR& get() const noexcept;

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
