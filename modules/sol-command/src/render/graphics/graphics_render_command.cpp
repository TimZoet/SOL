#include "sol-command/render/graphics/graphics_render_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_frame_buffer.h"
#include "sol-core/vulkan_render_pass.h"
#include "sol-error/sol_error.h"
#include "sol-render/graphics/graphics_renderer.h"

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

    VulkanRenderPass* GraphicsRenderCommand::getRenderPass() const noexcept { return renderPass; }

    VulkanCommandBufferList* GraphicsRenderCommand::getCommandBufferList() const noexcept { return commandBuffers; }

    std::vector<VulkanFramebuffer*>& GraphicsRenderCommand::getFramebuffers() noexcept { return framebuffers; }

    const std::vector<VulkanFramebuffer*>& GraphicsRenderCommand::getFramebuffers() const noexcept
    {
        return framebuffers;
    }

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

    void GraphicsRenderCommand::setRenderPass(VulkanRenderPass& pass)
    {
        commandQueue->requireNonFinalized();
        renderPass = &pass;
    }

    void GraphicsRenderCommand::setCommandBufferList(VulkanCommandBufferList& buffers)
    {
        commandQueue->requireNonFinalized();
        commandBuffers = &buffers;
    }

    void GraphicsRenderCommand::addFramebuffer(VulkanFramebuffer& framebuffer)
    {
        commandQueue->requireNonFinalized();
        framebuffers.emplace_back(&framebuffer);
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
        if (!renderData) throw SolError("Cannot finalize GraphicsRenderCommand: renderData not set.");
        if (!renderPass) throw SolError("Cannot finalize GraphicsRenderCommand: renderpass not set.");
        if (!commandBuffers) throw SolError("Cannot finalize GraphicsRenderCommand: commandBuffers not set.");
        if (framebuffers.empty()) throw SolError("Cannot finalize GraphicsRenderCommand: framebuffer not set.");
        if (framebuffers.size() > 1 && !imageIndexPtr)
            throw SolError(
              "Cannot finalize GraphicsRenderCommand: imageIndexPtr is null while there are more than 1 framebuffers.");
    }

    void GraphicsRenderCommand::operator()()
    {
        // Get framebuffer.
        const auto* framebuffer = framebuffers.front();
        if (framebuffers.size() > 1)
        {
            if (*imageIndexPtr >= framebuffers.size())
                throw SolError("Cannot run GraphicsRenderCommand: imageIndexPtr is out of bounds.");

            framebuffer = framebuffers[*imageIndexPtr];
        }

        commandBuffers->resetCommand(*frameIndexPtr, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        commandBuffers->beginOneTimeCommand(*frameIndexPtr);

        const GraphicsRenderer::Parameters params = {.renderData    = *renderData,
                                                     .renderPass    = *renderPass,
                                                     .framebuffer   = *framebuffer,
                                                     .commandBuffer = commandBuffers->get(*frameIndexPtr),
                                                     .index         = *frameIndexPtr};
        renderer->createPipelines(params);
        renderer->beginRenderPass(params);
        renderer->render(params);
        renderer->endRenderPass(params);

        commandBuffers->endCommand(*frameIndexPtr);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string GraphicsRenderCommand::getVizLabel() const { return "GraphicsRender"; }
}  // namespace sol
