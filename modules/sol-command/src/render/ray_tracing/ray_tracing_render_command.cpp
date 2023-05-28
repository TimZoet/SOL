#include "sol-command/render/ray_tracing/ray_tracing_render_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_frame_buffer.h"
#include "sol-core/vulkan_render_pass.h"
#include "sol-error/sol_error.h"
#include "sol-render/ray_tracing/ray_tracing_renderer.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingRenderCommand::RayTracingRenderCommand() = default;

    RayTracingRenderCommand::~RayTracingRenderCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    RayTracingRenderer* RayTracingRenderCommand::getRenderer() const noexcept { return renderer; }

    RayTracingRenderData* RayTracingRenderCommand::getRenderData() const noexcept { return renderData; }

    VulkanCommandBufferList* RayTracingRenderCommand::getCommandBufferList() const noexcept { return commandBuffers; }

    const uint32_t* RayTracingRenderCommand::getFrameIndexPtr() const noexcept { return frameIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RayTracingRenderCommand::setRenderer(RayTracingRenderer& render)
    {
        commandQueue->requireNonFinalized();
        renderer = &render;
    }

    void RayTracingRenderCommand::setRenderData(RayTracingRenderData& data)
    {
        commandQueue->requireNonFinalized();
        renderData = &data;
    }

    void RayTracingRenderCommand::setCommandBufferList(VulkanCommandBufferList& buffers)
    {
        commandQueue->requireNonFinalized();
        commandBuffers = &buffers;
    }

    void RayTracingRenderCommand::setFrameIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        frameIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void RayTracingRenderCommand::finalize()
    {
        if (!renderer) throw SolError("Cannot finalize RayTracingRenderCommand: renderer not set.");
        if (!renderData) throw SolError("Cannot finalize RayTracingRenderCommand: renderData not set.");
        if (!commandBuffers) throw SolError("Cannot finalize RayTracingRenderCommand: commandBuffers not set.");
    }

    void RayTracingRenderCommand::operator()()
    {
        commandBuffers->resetCommand(*frameIndexPtr, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        commandBuffers->beginOneTimeCommand(*frameIndexPtr);

        const RayTracingRenderer::Parameters params = {
          .renderData = *renderData, .commandBuffer = commandBuffers->get(*frameIndexPtr), .index = *frameIndexPtr};
        renderer->createPipelines(params);
        renderer->render(params);

        commandBuffers->endCommand(*frameIndexPtr);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string RayTracingRenderCommand::getVizLabel() const { return "RayTracingRender"; }
}  // namespace sol
