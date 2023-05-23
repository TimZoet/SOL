#include "sol-command/render/compute/compute_render_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_frame_buffer.h"
#include "sol-core/vulkan_render_pass.h"
#include "sol-error/sol_error.h"
#include "sol-render/compute/compute_renderer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeRenderCommand::ComputeRenderCommand() = default;

    ComputeRenderCommand::~ComputeRenderCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ComputeRenderer* ComputeRenderCommand::getRenderer() const noexcept { return renderer; }

    ComputeRenderData* ComputeRenderCommand::getRenderData() const noexcept { return renderData; }

    VulkanCommandBufferList* ComputeRenderCommand::getCommandBufferList() const noexcept { return commandBuffers; }

    const uint32_t* ComputeRenderCommand::getFrameIndexPtr() const noexcept { return frameIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ComputeRenderCommand::setRenderer(ComputeRenderer& render)
    {
        commandQueue->requireNonFinalized();
        renderer = &render;
    }

    void ComputeRenderCommand::setRenderData(ComputeRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void ComputeRenderCommand::setCommandBufferList(VulkanCommandBufferList& buffers)
    {
        commandQueue->requireNonFinalized();
        commandBuffers = &buffers;
    }

    void ComputeRenderCommand::setFrameIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        frameIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void ComputeRenderCommand::finalize()
    {
        if (!renderer) throw SolError("Cannot finalize ComputeRenderCommand: renderer not set.");
        if (!renderData) throw SolError("Cannot finalize ComputeRenderCommand: renderData not set.");
        if (!commandBuffers) throw SolError("Cannot finalize ComputeRenderCommand: commandBuffers not set.");
    }

    void ComputeRenderCommand::operator()()
    {
        commandBuffers->resetCommand(*frameIndexPtr, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        commandBuffers->beginOneTimeCommand(*frameIndexPtr);

        const ComputeRenderer::Parameters params = {
          .renderData = *renderData, .commandBuffer = commandBuffers->get(*frameIndexPtr), .index = *frameIndexPtr};
        renderer->createPipelines(params);
        renderer->render(params);

        commandBuffers->endCommand(*frameIndexPtr);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string ComputeRenderCommand::getVizLabel() const { return "ComputeRender"; }
}  // namespace sol
