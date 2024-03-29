#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>
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
#include "sol-core/vulkan_physical_device_features.h"

namespace sol
{
    /**
     * \brief The VulkanDevice object manages the lifetime of a VkDevice.
     */
    class VulkanDevice
    {
    public:
        /**
         * \brief VulkanDevice settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan physical device.
             */
            ObjectRefSetting<VulkanPhysicalDevice> physicalDevice;

            /**
             * \brief Optional features to enable.
             */
            RootVulkanPhysicalDeviceFeatures2* features = nullptr;

            /**
             * \brief List of required device extensions.
             */
            std::vector<std::string> extensions;

            /**
             * \brief Number of queues to create for each queue family.
             */
            std::vector<uint32_t> queues;

            /**
             * \brief If true, makes all queues thread safe on construction.
             */
            bool threadSafeQueues = false;

            [[nodiscard]] bool validate() const noexcept;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanDevice() = delete;

        VulkanDevice(const Settings& set, VkDevice vkDevice);

        VulkanDevice(const VulkanDevice&) = delete;

        VulkanDevice(VulkanDevice&&) = delete;

        ~VulkanDevice() noexcept;

        VulkanDevice& operator=(const VulkanDevice&) = delete;

        VulkanDevice& operator=(VulkanDevice&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan device.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \return Vulkan device.
         */
        [[nodiscard]] static VulkanDevicePtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan device.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \return Vulkan device.
         */
        [[nodiscard]] static VulkanDeviceSharedPtr createShared(const Settings& settings);

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
         * \brief Get the physical device.
         * \return VulkanPhysicalDevice.
         */
        [[nodiscard]] VulkanPhysicalDevice& getPhysicalDevice() noexcept;

        /**
         * \brief Get the physical device.
         * \return VulkanPhysicalDevice.
         */
        [[nodiscard]] const VulkanPhysicalDevice& getPhysicalDevice() const noexcept;

        /**
         * \brief Get the device handle managed by this object.
         * \return Device handle.
         */
        [[nodiscard]] const VkDevice& get() const noexcept;

        /**
         * \brief Get the list of all queues created for this device.
         * \return List of queues.
         */
        [[nodiscard]] const std::vector<VulkanQueuePtr>& getQueues() const noexcept;

        /**
         * \brief Get the list of queues of a specific family created for this device.
         * \param family Queue family.
         * \return List of queues.
         */
        [[nodiscard]] std::vector<VulkanQueue*> getQueues(const VulkanQueueFamily& family) const;

    private:
        [[nodiscard]] static VkDevice createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanPhysicalDevice* physicalDevice = nullptr;
#endif

        /**
         * \brief Vulkan device.
         */
        VkDevice device = VK_NULL_HANDLE;

        /**
         * \brief List of queues.
         */
        std::vector<VulkanQueuePtr> queues;

        ////////////////////////////////////////////////////////////////
        // Extension functions.
        ////////////////////////////////////////////////////////////////

        void loadExtensionFunctions(const Settings& set);

    public:
        /* VK_EXT_descriptor_buffer */

        PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT = VK_NULL_HANDLE;
        PFN_vkCmdBindDescriptorBuffersEXT                vkCmdBindDescriptorBuffersEXT                = VK_NULL_HANDLE;
        PFN_vkCmdSetDescriptorBufferOffsetsEXT           vkCmdSetDescriptorBufferOffsetsEXT           = VK_NULL_HANDLE;
        PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT    vkGetBufferOpaqueCaptureDescriptorDataEXT    = VK_NULL_HANDLE;
        PFN_vkGetDescriptorEXT                           vkGetDescriptorEXT                           = VK_NULL_HANDLE;
        PFN_vkGetDescriptorSetLayoutBindingOffsetEXT     vkGetDescriptorSetLayoutBindingOffsetEXT     = VK_NULL_HANDLE;
        PFN_vkGetDescriptorSetLayoutSizeEXT              vkGetDescriptorSetLayoutSizeEXT              = VK_NULL_HANDLE;
        PFN_vkGetImageOpaqueCaptureDescriptorDataEXT     vkGetImageOpaqueCaptureDescriptorDataEXT     = VK_NULL_HANDLE;
        PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT = VK_NULL_HANDLE;
        PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT   vkGetSamplerOpaqueCaptureDescriptorDataEXT   = VK_NULL_HANDLE;

        /* VK_EXT_extended_dynamic_state3 */

        PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT = VK_NULL_HANDLE;

        /* VK_KHR_acceleration_structure */

        PFN_vkCmdBuildAccelerationStructuresKHR        vkCmdBuildAccelerationStructuresKHR        = VK_NULL_HANDLE;
        PFN_vkCreateAccelerationStructureKHR           vkCreateAccelerationStructureKHR           = VK_NULL_HANDLE;
        PFN_vkDestroyAccelerationStructureKHR          vkDestroyAccelerationStructureKHR          = VK_NULL_HANDLE;
        PFN_vkGetAccelerationStructureBuildSizesKHR    vkGetAccelerationStructureBuildSizesKHR    = VK_NULL_HANDLE;
        PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = VK_NULL_HANDLE;

        /* VK_KHR_ray_tracing_pipeline */

        PFN_vkCreateRayTracingPipelinesKHR       vkCreateRayTracingPipelinesKHR       = VK_NULL_HANDLE;
        PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = VK_NULL_HANDLE;
        PFN_vkCmdTraceRaysKHR                    vkCmdTraceRaysKHR                    = VK_NULL_HANDLE;
    };
}  // namespace sol
