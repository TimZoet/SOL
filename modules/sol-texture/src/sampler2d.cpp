#include "sol-texture/sampler2d.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid_system_generator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_sampler.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Sampler2D::Sampler2D(const uuids::uuid id, VulkanSamplerPtr s) : uuid(id), sampler(std::move(s)) {}

    Sampler2D::Sampler2D(VulkanSamplerPtr s) : uuid(uuids::uuid_system_generator{}()), sampler(std::move(s)) {}

    Sampler2D::~Sampler2D() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const uuids::uuid& Sampler2D::getUuid() const noexcept { return uuid; }

    VulkanSampler& Sampler2D::getSampler() noexcept { return *sampler; }

    const VulkanSampler& Sampler2D::getSampler() const noexcept { return *sampler; }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Sampler2DPtr Sampler2D::create(const Settings& settings, uuids::uuid id)
    {
        VulkanSampler::Settings samplerSettings;
        samplerSettings.device       = settings.device;
        samplerSettings.magFilter    = settings.magFilter;
        samplerSettings.minFilter    = settings.minFilter;
        samplerSettings.mipmapMode   = settings.mipmapMode;
        samplerSettings.addressModeU = settings.addressModeU;
        samplerSettings.addressModeV = settings.addressModeV;
        samplerSettings.addressModeW = settings.addressModeW;
        auto vulkanSampler           = VulkanSampler::create(samplerSettings);

        if (id.is_nil()) return std::make_unique<Sampler2D>(std::move(vulkanSampler));

        return std::make_unique<Sampler2D>(id, std::move(vulkanSampler));
    }
}  // namespace sol
