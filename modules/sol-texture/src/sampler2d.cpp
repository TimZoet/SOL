#include "sol-texture/sampler2d.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_sampler.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Sampler2D::Sampler2D(TextureCollection& collection, const uuids::uuid id, VulkanSamplerPtr s) :
        textureCollection(&collection), uuid(id), sampler(std::move(s))
    {
    }

    Sampler2D::~Sampler2D() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    TextureCollection& Sampler2D::getTextureCollection() noexcept { return *textureCollection; }

    const TextureCollection& Sampler2D::getTextureCollection() const noexcept { return *textureCollection; }

    const uuids::uuid& Sampler2D::getUuid() const noexcept { return uuid; }

    VulkanSampler& Sampler2D::getSampler() noexcept { return *sampler; }

    const VulkanSampler& Sampler2D::getSampler() const noexcept { return *sampler; }

}  // namespace sol
