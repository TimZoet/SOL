#include "sol-core/vulkan_sampler.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanSampler::VulkanSampler(const Settings& set, const VkSampler vkSampler) : settings(set), sampler(vkSampler) {}
#else
    VulkanSampler::VulkanSampler(const Settings& set, const VkSampler vkSampler) :
        device(&set.device()), sampler(vkSampler)
    {
    }
#endif

    VulkanSampler::~VulkanSampler() noexcept { vkDestroySampler(getDevice().get(), sampler, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSamplerPtr VulkanSampler::create(const Settings& settings)
    {
        auto sampler = createImpl(settings);
        return std::make_unique<VulkanSampler>(settings, sampler);
    }

    VulkanSamplerSharedPtr VulkanSampler::createShared(const Settings& settings)
    {
        auto sampler = createImpl(settings);
        return std::make_shared<VulkanSampler>(settings, sampler);
    }

    VkSampler VulkanSampler::createImpl(const Settings& settings)
    {
        // Prepare creation info.
        VkSamplerCreateInfo createInfo;
        createInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.pNext                   = nullptr;
        createInfo.flags                   = 0;
        createInfo.magFilter               = settings.magFilter;
        createInfo.minFilter               = settings.minFilter;
        createInfo.mipmapMode              = settings.mipmapMode;
        createInfo.addressModeU            = settings.addressModeU;
        createInfo.addressModeV            = settings.addressModeV;
        createInfo.addressModeW            = settings.addressModeW;
        createInfo.mipLodBias              = settings.mipLodBias;
        createInfo.anisotropyEnable        = settings.anisotropyEnable;
        createInfo.maxAnisotropy           = settings.maxAnisotropy;
        createInfo.compareEnable           = settings.compareEnable;
        createInfo.compareOp               = settings.compareOp;
        createInfo.minLod                  = settings.minLod;
        createInfo.maxLod                  = settings.maxLod;
        createInfo.borderColor             = settings.borderColor;
        createInfo.unnormalizedCoordinates = settings.unnormalizedCoordinates;

        // Create sampler.
        VkSampler vkSampler;
        handleVulkanError(vkCreateSampler(settings.device, &createInfo, nullptr, &vkSampler));

        return vkSampler;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanSampler::Settings& VulkanSampler::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanSampler::getDevice() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VulkanDevice& VulkanSampler::getDevice() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.device();
#else
        return *device;
#endif
    }

    const VkSampler& VulkanSampler::get() const noexcept { return sampler; }
}  // namespace sol