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
    class IGraphicsMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IGraphicsMaterialManager();

        IGraphicsMaterialManager(const IGraphicsMaterialManager&) = delete;

        IGraphicsMaterialManager(IGraphicsMaterialManager&&) = delete;

        virtual ~IGraphicsMaterialManager() noexcept;

        IGraphicsMaterialManager& operator=(const IGraphicsMaterialManager&) = delete;

        IGraphicsMaterialManager& operator=(IGraphicsMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Interface.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual VulkanGraphicsPipeline& getPipeline(const GraphicsMaterial& material) const = 0;

        virtual bool createPipeline(const GraphicsMaterial& material) const = 0;

        virtual void bindDescriptorSets(std::span<const GraphicsMaterialInstance* const> instances,
                                        VkCommandBuffer                                  commandBuffer,
                                        const VulkanGraphicsPipeline&                    pipeline,
                                        size_t                                           index) const = 0;

        virtual void updateUniformBuffers(uint32_t index) = 0;
    };
}  // namespace sol
