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

    VulkanSampler::VulkanSampler(SettingsPtr settingsPtr, const VkSampler vkSampler) :
        settings(std::move(settingsPtr)), sampler(vkSampler)
    {
    }

    VulkanSampler::~VulkanSampler() noexcept { vkDestroySampler(settings->device, sampler, nullptr); }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanSamplerPtr VulkanSampler::create(Settings settings)
    {
        auto sampler = createImpl(settings);
        return std::make_unique<VulkanSampler>(std::make_unique<Settings>(settings), sampler);
    }

    VulkanSamplerSharedPtr VulkanSampler::createShared(Settings settings)
    {
        auto sampler = createImpl(settings);
        return std::make_shared<VulkanSampler>(std::make_unique<Settings>(settings), sampler);
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

    const VulkanSampler::Settings& VulkanSampler::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanSampler::getDevice() noexcept { return settings->device(); }

    const VulkanDevice& VulkanSampler::getDevice() const noexcept { return settings->device(); }

    const VkSampler& VulkanSampler::get() const noexcept { return sampler; }
}  // namespace sol