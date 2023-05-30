#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <span>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

namespace sol
{
    class IForwardMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IForwardMaterialManager();

        IForwardMaterialManager(const IForwardMaterialManager&) = delete;

        IForwardMaterialManager(IForwardMaterialManager&&) = delete;

        virtual ~IForwardMaterialManager() noexcept;

        IForwardMaterialManager& operator=(const IForwardMaterialManager&) = delete;

        IForwardMaterialManager& operator=(IForwardMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Interface.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual VulkanGraphicsPipeline& getPipeline(const ForwardMaterial&  material,
                                                                  const VulkanRenderPass& renderPass) const = 0;

        virtual bool createPipeline(const ForwardMaterial& material, VulkanRenderPass& renderPass) const = 0;

        virtual void bindDescriptorSets(std::span<const ForwardMaterialInstance* const> instances,
                                        VkCommandBuffer                                 commandBuffer,
                                        const VulkanGraphicsPipeline&                   pipeline,
                                        size_t                                          index) const = 0;

        virtual void updateUniformBuffers(uint32_t index) = 0;
    };
}  // namespace sol
