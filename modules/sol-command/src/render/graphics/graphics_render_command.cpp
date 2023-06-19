#include "sol-command/render/graphics/graphics_render_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-error/sol_error.h"
#include "sol-render/graphics/graphics_renderer.h"
#include "sol-render/graphics/graphics_rendering_info.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderCommand::GraphicsRenderCommand() = default;

    GraphicsRenderCommand::~GraphicsRenderCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderer* GraphicsRenderCommand::getRenderer() const noexcept { return renderer; }

    const std::vector<GraphicsRenderingInfo*>& GraphicsRenderCommand::getRenderingInfos() const noexcept
    {
        return renderingInfos;
    }

    VulkanCommandBufferList* GraphicsRenderCommand::getCommandBufferList() const noexcept { return commandBuffers; }

    const uint32_t* GraphicsRenderCommand::getFrameIndexPtr() const noexcept { return frameIndexPtr; }

    const uint32_t* GraphicsRenderCommand::getImageIndexPtr() const noexcept { return imageIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderCommand::setRenderer(GraphicsRenderer& render)
    {
        commandQueue->requireNonFinalized();
        renderer = &render;
    }

    void GraphicsRenderCommand::setRenderData(GraphicsRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void GraphicsRenderCommand::addRenderingInfo(GraphicsRenderingInfo& info) { renderingInfos.emplace_back(&info); }

    void GraphicsRenderCommand::setCommandBufferList(VulkanCommandBufferList& buffers)
    {
        commandQueue->requireNonFinalized();
        commandBuffers = &buffers;
    }

    void GraphicsRenderCommand::setFrameIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        frameIndexPtr = ptr;
    }

    void GraphicsRenderCommand::setImageIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        imageIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderCommand::finalize()
    {
        if (!renderer) throw SolError("Cannot finalize GraphicsRenderCommand: renderer not set.");
        if (renderingInfos.empty()) throw SolError("Cannot finalize GraphicsRenderCommand: renderingInfo not set.");
        if (renderingInfos.size() > 1 && !imageIndexPtr)
            throw SolError("Cannot finalize GraphicsRenderCommand: imageIndexPtr is null while there are more than 1 "
                           "renderingInfos.");
        if (!renderData) throw SolError("Cannot finalize GraphicsRenderCommand: renderData not set.");
        if (!commandBuffers) throw SolError("Cannot finalize GraphicsRenderCommand: commandBuffers not set.");
    }

    void GraphicsRenderCommand::operator()()
    {
        // Get render info for current frame.
        const auto* renderingInfo = renderingInfos.front();
        if (renderingInfos.size() > 1)
        {
            if (*imageIndexPtr >= renderingInfos.size())
                throw SolError("Cannot run GraphicsRenderCommand: imageIndexPtr is out of bounds.");

            renderingInfo = renderingInfos[*imageIndexPtr];
        }

        commandBuffers->resetCommand(*frameIndexPtr, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        commandBuffers->beginOneTimeCommand(*frameIndexPtr);

        const GraphicsRenderer::Parameters params = {.device        = commandBuffers->getDevice(),
                                                     .renderData    = *renderData,
                                                     .commandBuffer = commandBuffers->get(*frameIndexPtr),
                                                     .index         = *frameIndexPtr};
        renderer->createPipelines(params);

        // TODO: Yuck! Hardcoded barriers. That should change sooner rather than later...
        {
            VkImageMemoryBarrier2 barrier{};
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            barrier.srcAccessMask                   = 0;
            barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            barrier.dstAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.image                           = renderingInfo->getColorImageViews()[0]->getImage().get();
            barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount     = 1;
            barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            VkDependencyInfo info{};
            info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            info.imageMemoryBarrierCount = 1;
            info.pImageMemoryBarriers    = &barrier;
            vkCmdPipelineBarrier2(commandBuffers->get(*frameIndexPtr), &info);
        }

        vkCmdBeginRendering(commandBuffers->get(*frameIndexPtr), &renderingInfo->get());
        renderer->render(params);
        vkCmdEndRendering(commandBuffers->get(*frameIndexPtr));

        {
            VkImageMemoryBarrier2 barrier{};
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            barrier.srcAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
            barrier.dstAccessMask                   = 0;
            barrier.oldLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            barrier.image                           = renderingInfo->getColorImageViews()[0]->getImage().get();
            barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount     = 1;
            barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            VkDependencyInfo info{};
            info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            info.imageMemoryBarrierCount = 1;
            info.pImageMemoryBarriers    = &barrier;
            vkCmdPipelineBarrier2(commandBuffers->get(*frameIndexPtr), &info);
        }

        commandBuffers->endCommand(*frameIndexPtr);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string GraphicsRenderCommand::getVizLabel() const { return "GraphicsRender"; }
}  // namespace sol
