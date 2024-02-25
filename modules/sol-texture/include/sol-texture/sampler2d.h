#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

namespace sol
{
    class Sampler2D
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Settings
        {
            ObjectRefSetting<VulkanDevice> device;
            VkFilter                       magFilter    = VK_FILTER_LINEAR;
            VkFilter                       minFilter    = VK_FILTER_LINEAR;
            VkSamplerMipmapMode            mipmapMode   = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            VkSamplerAddressMode           addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            VkSamplerAddressMode           addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            VkSamplerAddressMode           addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Sampler2D() = delete;

        Sampler2D(uuids::uuid id, VulkanSamplerPtr s);

        explicit Sampler2D(VulkanSamplerPtr s);

        Sampler2D(const Sampler2D&) = delete;

        Sampler2D(Sampler2D&&) = delete;

        ~Sampler2D() noexcept;

        Sampler2D& operator=(const Sampler2D&) = delete;

        Sampler2D& operator=(Sampler2D&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        [[nodiscard]] VulkanSampler& getSampler() noexcept;

        [[nodiscard]] const VulkanSampler& getSampler() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new 2D sampler.
         * \param settings Settings.
         * \param id Identifier. If empty, generated automatically.
         * \return New Sampler2D.
         */
        [[nodiscard]] static Sampler2DPtr create(const Settings& settings, uuids::uuid id = uuids::uuid{});

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        uuids::uuid uuid;

        VulkanSamplerPtr sampler;
    };
}  // namespace sol
