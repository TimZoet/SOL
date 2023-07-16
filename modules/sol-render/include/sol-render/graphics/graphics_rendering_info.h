#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cstdint>
#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

namespace sol
{
    class GraphicsRenderingInfo
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct ColorAttachmentTransition
        {
            const VulkanQueueFamily* srcQueue;
            const VulkanQueueFamily* dstQueue;
            VkImageLayout            oldLayout;
            VkImageLayout            newLayout;
            VkPipelineStageFlags2    srcStage;
            VkPipelineStageFlags2    dstStage;
            VkAccessFlags2           srcAccess;
            VkAccessFlags2           dstAccess;
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsRenderingInfo();

        GraphicsRenderingInfo(const GraphicsRenderingInfo&) = delete;

        GraphicsRenderingInfo(GraphicsRenderingInfo&&) = delete;

        ~GraphicsRenderingInfo() noexcept;

        GraphicsRenderingInfo& operator=(const GraphicsRenderingInfo&) = delete;

        GraphicsRenderingInfo& operator=(GraphicsRenderingInfo&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool isFinalized() const noexcept;

        void requireFinalized() const;

        void requireNonFinalized() const;

        [[nodiscard]] const VkRenderingInfo& get() const;

        [[nodiscard]] const std::vector<const VulkanImageView*>& getColorImageViews() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        void setRenderArea(VkRect2D area);

        void setRenderArea(VkOffset2D offset, VkExtent2D extent);

        void setRenderArea(int32_t offsetX, int32_t offsetY, uint32_t extentX, uint32_t extentY);

        void setLayerCount(uint32_t count);

        void addColorAttachment(const VulkanImageView& imageView,
                                VkImageLayout          imageLayout,
                                VkAttachmentLoadOp     loadOp,
                                VkAttachmentStoreOp    storeOp,
                                std::array<float, 4>   clearColor);

        void addColorAttachment(const VulkanImageView& imageView,
                                VkImageLayout          imageLayout,
                                VkAttachmentLoadOp     loadOp,
                                VkAttachmentStoreOp    storeOp,
                                std::array<int32_t, 4> clearColor);

        void addColorAttachment(const VulkanImageView&  imageView,
                                VkImageLayout           imageLayout,
                                VkAttachmentLoadOp      loadOp,
                                VkAttachmentStoreOp     storeOp,
                                std::array<uint32_t, 4> clearColor);

        void setColorAttachmentPreTransition(size_t                   index,
                                             const VulkanQueueFamily* srcQueue,
                                             const VulkanQueueFamily* dstQueue,
                                             VkImageLayout            oldLayout,
                                             VkImageLayout            newLayout,
                                             VkPipelineStageFlags2    srcStage,
                                             VkPipelineStageFlags2    dstStage,
                                             VkAccessFlags2           srcAccess,
                                             VkAccessFlags2           dstAccess);

        void setColorAttachmentPostTransition(size_t                   index,
                                              const VulkanQueueFamily* srcQueue,
                                              const VulkanQueueFamily* dstQueue,
                                              VkImageLayout            oldLayout,
                                              VkImageLayout            newLayout,
                                              VkPipelineStageFlags2    srcStage,
                                              VkPipelineStageFlags2    dstStage,
                                              VkAccessFlags2           srcAccess,
                                              VkAccessFlags2           dstAccess);

        void addDepthAttachment(const VulkanImageView& imageView,
                                VkImageLayout          imageLayout,
                                VkAttachmentLoadOp     loadOp,
                                VkAttachmentStoreOp    storeOp,
                                float                  clearDepth);

        void addStencilAttachment(const VulkanImageView& imageView,
                                  VkImageLayout          imageLayout,
                                  VkAttachmentLoadOp     loadOp,
                                  VkAttachmentStoreOp    storeOp,
                                  uint32_t               clearStencil);

        void finalize();

        ////////////////////////////////////////////////////////////////
        // Commands.
        ////////////////////////////////////////////////////////////////

        void beginRendering(const VulkanCommandBuffer& buffer) const;

        void endRendering(const VulkanCommandBuffer& buffer) const;

        void preTransition(const VulkanCommandBuffer& buffer) const;

        void postTransition(const VulkanCommandBuffer& buffer) const;

    private:
        void addColorAttachment(const VulkanImageView& imageView,
                                VkImageLayout          imageLayout,
                                VkAttachmentLoadOp     loadOp,
                                VkAttachmentStoreOp    storeOp,
                                VkClearColorValue      clearColor);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_MAX_ENUM};

        VkRect2D renderArea{};
        uint32_t layerCount = 0;

        std::vector<const VulkanImageView*>      colorImageViews;
        std::vector<VkRenderingAttachmentInfo>   colorAttachmentInfo;
        std::optional<VkRenderingAttachmentInfo> depthAttachmentInfo;
        std::optional<VkRenderingAttachmentInfo> stencilAttachmentInfo;

        std::vector<std::optional<ColorAttachmentTransition>> preColorTransitions;
        std::vector<std::optional<ColorAttachmentTransition>> postColorTransitions;

        std::vector<VkImageMemoryBarrier2> preBarriers;
        std::vector<VkImageMemoryBarrier2> postBarriers;
        VkDependencyInfo                   preDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
        VkDependencyInfo                   postDependencyInfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    };
}  // namespace sol
