#include "sol-command/render/forward/forward_render_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_frame_buffer.h"
#include "sol-core/vulkan_render_pass.h"
#include "sol-error/sol_error.h"
#include "sol-render/forward/forward_renderer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardRenderCommand::ForwardRenderCommand() = default;

    ForwardRenderCommand::~ForwardRenderCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ForwardRenderer* ForwardRenderCommand::getRenderer() const noexcept { return renderer; }

    VulkanRenderPass* ForwardRenderCommand::getRenderPass() const noexcept { return renderPass; }

    VulkanCommandBufferList* ForwardRenderCommand::getCommandBufferList() const noexcept { return commandBuffers; }

    std::vector<VulkanFramebuffer*>& ForwardRenderCommand::getFramebuffers() noexcept { return framebuffers; }

    const std::vector<VulkanFramebuffer*>& ForwardRenderCommand::getFramebuffers() const noexcept
    {
        return framebuffers;
    }

    const uint32_t* ForwardRenderCommand::getImageIndexPtr() const noexcept { return imageIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ForwardRenderCommand::setRenderer(ForwardRenderer& render)
    {
        commandQueue->requireNonFinalized();
        renderer = &render;
    }

    void ForwardRenderCommand::setRenderData(ForwardRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void ForwardRenderCommand::setRenderPass(VulkanRenderPass& pass)
    {
        commandQueue->requireNonFinalized();
        renderPass = &pass;
    }

    void ForwardRenderCommand::setCommandBufferList(VulkanCommandBufferList& buffers)
    {
        commandQueue->requireNonFinalized();
        commandBuffers = &buffers;
    }

    void ForwardRenderCommand::addFramebuffer(VulkanFramebuffer& framebuffer)
    {
        commandQueue->requireNonFinalized();
        framebuffers.emplace_back(&framebuffer);
    }

    void ForwardRenderCommand::setImageIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        imageIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void ForwardRenderCommand::finalize()
    {
        if (!renderer) throw SolError("Cannot finalize ForwardRenderCommand: renderer not set.");
        if (!renderData) throw SolError("Cannot finalize ForwardRenderCommand: renderData not set.");
        if (!renderPass) throw SolError("Cannot finalize ForwardRenderCommand: renderpass not set.");
        if (!commandBuffers) throw SolError("Cannot finalize ForwardRenderCommand: commandBuffers not set.");
        if (framebuffers.empty()) throw SolError("Cannot finalize ForwardRenderCommand: framebuffer not set.");
        if (framebuffers.size() > 1 && !imageIndexPtr)
            throw SolError(
              "Cannot finalize ForwardRenderCommand: imageIndexPtr is null while there are more than 1 framebuffers.");
    }

    void ForwardRenderCommand::operator()()
    {
        // Get framebuffer.
        const auto* framebuffer = framebuffers.front();
        if (framebuffers.size() > 1)
        {
            if (*imageIndexPtr >= framebuffers.size())
                throw SolError("Cannot run ForwardRenderCommand: imageIndexPtr is out of bounds.");

            framebuffer = framebuffers[*imageIndexPtr];
        }

        commandBuffers->resetCommand(*imageIndexPtr, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        commandBuffers->beginOneTimeCommand(*imageIndexPtr);

        const ForwardRenderer::Parameters params = {.renderData    = *renderData,
                                                    .renderPass    = *renderPass,
                                                    .framebuffer   = *framebuffer,
                                                    .commandBuffer = commandBuffers->get(*imageIndexPtr),
                                                    .index         = *imageIndexPtr};
        renderer->createPipelines(params);
        renderer->beginRenderPass(params);
        renderer->render(params);
        renderer->endRenderPass(params);

        commandBuffers->endCommand(*imageIndexPtr);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string ForwardRenderCommand::getVizLabel() const { return "ForwardRender"; }
}  // namespace sol
