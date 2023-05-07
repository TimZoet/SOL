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
    class VulkanSampler
    {
    public:
        /**
         * \brief VulkanSampler settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            VkFilter magFilter = VK_FILTER_NEAREST;

            VkFilter minFilter = VK_FILTER_NEAREST;

            VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

            VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

            VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

            VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

            float mipLodBias = 0.0f;

            VkBool32 anisotropyEnable = VK_FALSE;

            float maxAnisotropy = 1.0f;

            VkBool32 compareEnable = VK_FALSE;

            VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;

            float minLod = 0.0f;

            float maxLod = 0.0f;

            VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

            VkBool32 unnormalizedCoordinates = VK_FALSE;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanSampler() = delete;

        VulkanSampler(const Settings& set, VkSampler vkSampler);

        VulkanSampler(const VulkanSampler&) = delete;

        VulkanSampler(VulkanSampler&&) = delete;

        ~VulkanSampler() noexcept;

        VulkanSampler& operator=(const VulkanSampler&) = delete;

        VulkanSampler& operator=(VulkanSampler&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan sampler.
         * \param settings Settings.
         * \throws VulkanError Thrown if sampler creation failed.
         * \return Vulkan image view.
         */
        [[nodiscard]] static VulkanSamplerPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan sampler.
         * \param settings Settings.
         * \throws VulkanError Thrown if sampler creation failed.
         * \return Vulkan sampler.
         */
        [[nodiscard]] static VulkanSamplerSharedPtr createShared(const Settings& settings);

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
         * \brief Get the sampler handle managed by this object.
         * \return Sampler handle.
         */
        [[nodiscard]] const VkSampler& get() const noexcept;

    private:
        [[nodiscard]] static VkSampler createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanDevice* device = nullptr;
#endif

        /**
         * \brief Vulkan sampler.
         */
        VkSampler sampler = VK_NULL_HANDLE;
    };
}  // namespace sol