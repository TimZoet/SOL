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
    class IRayTracingMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IRayTracingMaterialManager();

        IRayTracingMaterialManager(const IRayTracingMaterialManager&) = delete;

        IRayTracingMaterialManager(IRayTracingMaterialManager&&) = delete;

        virtual ~IRayTracingMaterialManager() noexcept;

        IRayTracingMaterialManager& operator=(const IRayTracingMaterialManager&) = delete;

        IRayTracingMaterialManager& operator=(IRayTracingMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Interface.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual VulkanRayTracingPipeline& getPipeline(const RayTracingMaterial& material) const = 0;

        virtual bool createPipeline(const RayTracingMaterial& material) const = 0;

        virtual void bindDescriptorSets(std::span<const RayTracingMaterialInstance* const> instances,
                                        VkCommandBuffer                                    commandBuffer,
                                        const VulkanRayTracingPipeline&                    pipeline,
                                        size_t                                             index) const = 0;
    };
}  // namespace sol
