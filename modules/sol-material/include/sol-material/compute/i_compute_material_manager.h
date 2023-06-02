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
    class IComputeMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IComputeMaterialManager();

        IComputeMaterialManager(const IComputeMaterialManager&) = delete;

        IComputeMaterialManager(IComputeMaterialManager&&) = delete;

        virtual ~IComputeMaterialManager() noexcept;

        IComputeMaterialManager& operator=(const IComputeMaterialManager&) = delete;

        IComputeMaterialManager& operator=(IComputeMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Interface.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual VulkanComputePipeline& getPipeline(const ComputeMaterial& material) const = 0;

        virtual bool createPipeline(const ComputeMaterial& material) const = 0;

        virtual void bindDescriptorSets(std::span<const ComputeMaterialInstance* const> instances,
                                        VkCommandBuffer                                 commandBuffer,
                                        const VulkanComputePipeline&                    pipeline,
                                        size_t                                          index) const = 0;
    };
}  // namespace sol
