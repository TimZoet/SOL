#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>

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
    class VulkanShaderBindingTable
    {
    public:
        /**
         * \brief VulkanShaderBindingTable settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Vulkan ray tracing pipeline.
             */
            ObjectRefSetting<VulkanRayTracingPipeline> pipeline;

            /**
             * \brief Optional allocator.
             */
            ObjectRefSetting<VulkanMemoryAllocator> allocator;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanShaderBindingTable() = delete;

        VulkanShaderBindingTable(const Settings&                 set,
                                 VulkanBufferPtr                 sbtBuffer,
                                 VkStridedDeviceAddressRegionKHR raygen,
                                 VkStridedDeviceAddressRegionKHR miss,
                                 VkStridedDeviceAddressRegionKHR hit,
                                 VkStridedDeviceAddressRegionKHR callable);

        VulkanShaderBindingTable(const VulkanShaderBindingTable&) = delete;

        VulkanShaderBindingTable(VulkanShaderBindingTable&&) = delete;

        ~VulkanShaderBindingTable() noexcept;

        VulkanShaderBindingTable& operator=(const VulkanShaderBindingTable&) = delete;

        VulkanShaderBindingTable& operator=(VulkanShaderBindingTable&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new shader binding table.
         * \param settings Settings.
         * \throws VulkanError Thrown if creation failed.
         * \return Shader binding table.
         */
        [[nodiscard]] static VulkanShaderBindingTablePtr create(const Settings& settings);

        /**
         * \brief Create a new shader binding table.
         * \param settings Settings.
         * \throws VulkanError Thrown if creation failed.
         * \return Shader binding table.
         */
        [[nodiscard]] static VulkanShaderBindingTableSharedPtr createShared(const Settings& settings);

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
         * \brief Get the address region of the raygen record(s).
         * \return Address region.
         */
        [[nodiscard]] VkStridedDeviceAddressRegionKHR getRaygenRegion() const noexcept;

        /**
         * \brief Get the address region of the miss record(s).
         * \return Address region.
         */
        [[nodiscard]] VkStridedDeviceAddressRegionKHR getMissRegion() const noexcept;

        /**
         * \brief Get the address region of the hit record(s).
         * \return Address region.
         */
        [[nodiscard]] VkStridedDeviceAddressRegionKHR getHitRegion() const noexcept;

        /**
         * \brief Get the address region of the callable record(s).
         * \return Address region.
         */
        [[nodiscard]] VkStridedDeviceAddressRegionKHR getCallableRegion() const noexcept;

    private:
        [[nodiscard]] static std::tuple<VulkanBufferPtr,
                                        VkStridedDeviceAddressRegionKHR,
                                        VkStridedDeviceAddressRegionKHR,
                                        VkStridedDeviceAddressRegionKHR,
                                        VkStridedDeviceAddressRegionKHR>
          createImpl(const Settings& settings);

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        /**
         * \brief Vulkan ray tracing pipeline.
         */
        VulkanRayTracingPipeline* pipeline = nullptr;
#endif

        /**
         * \brief Shader binding table buffer.
         */
        VulkanBufferPtr buffer;

        VkStridedDeviceAddressRegionKHR raygenRegion;

        VkStridedDeviceAddressRegionKHR missRegion;

        VkStridedDeviceAddressRegionKHR hitRegion;

        VkStridedDeviceAddressRegionKHR callableRegion;
    };
}  // namespace sol