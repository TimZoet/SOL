#pragma once

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

        VulkanShaderBindingTable(const Settings& set, std::vector<VulkanBufferPtr> b);

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

        [[nodiscard]] VkStridedDeviceAddressRegionKHR getRegion(size_t i) const noexcept;

    private:
        [[nodiscard]] static std::vector<VulkanBufferPtr> createImpl(const Settings& settings);

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

        std::vector<VulkanBufferPtr> buffers;

        std::vector<VkStridedDeviceAddressRegionKHR> regions;
    };
}  // namespace sol