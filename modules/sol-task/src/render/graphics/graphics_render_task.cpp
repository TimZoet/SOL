#include "sol-task/render/graphics/graphics_render_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-error/sol_error.h"
#include "sol-render/graphics/graphics_renderer.h"
#include "sol-render/graphics/graphics_rendering_info.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsRenderTask::GraphicsRenderTask() = default;

    GraphicsRenderTask::GraphicsRenderTask(std::string                           taskName,
                                           ITaskResource<GraphicsRenderer>*      rd,
                                           ITaskResource<GraphicsRenderData>*    rdData,
                                           ITaskResource<GraphicsRenderingInfo>* rdInfo,
                                           ITaskResource<VulkanCommandBuffer>*   cmdBuffer,
                                           ITaskResource<uint32_t>*              frameIdx) :
        ITask(std::move(taskName)),
        renderer(rd),
        renderData(rdData),
        renderingInfo(rdInfo),
        commandBuffer(cmdBuffer),
        frameIndex(frameIdx)
    {
    }

    GraphicsRenderTask::GraphicsRenderTask(GraphicsRenderTask&&) noexcept = default;

    GraphicsRenderTask::~GraphicsRenderTask() noexcept = default;

    GraphicsRenderTask& GraphicsRenderTask::operator=(GraphicsRenderTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void GraphicsRenderTask::finalize()
    {
        if (!renderer) throw SolError("Cannot finalize GraphicsRenderTask: renderer not set.");
        if (!renderData) throw SolError("Cannot finalize GraphicsRenderTask: renderData not set.");
        if (!renderingInfo) throw SolError("Cannot finalize GraphicsRenderTask: renderingInfo not set.");
        if (!commandBuffer) throw SolError("Cannot finalize GraphicsRenderTask: commandBuffer not set.");
        if (!frameIndex) throw SolError("Cannot finalize GraphicsRenderTask: frameIndex not set.");
    }

    void GraphicsRenderTask::operator()()
    {
        auto&       rRenderer      = **renderer;
        const auto& rRenderData    = **renderData;
        const auto& rRenderingInfo = **renderingInfo;
        const auto& rCommandBuffer = **commandBuffer;
        const auto  rFrameIndex    = **frameIndex;

        const GraphicsRenderer::Parameters params = {.device        = rCommandBuffer.getDevice(),
                                                     .renderData    = rRenderData,
                                                     .commandBuffer = rCommandBuffer.get(),
                                                     .index         = rFrameIndex};

        rCommandBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        rCommandBuffer.beginOneTimeCommand();
        rRenderer.createPipelines(params);
        rRenderingInfo.preTransition(rCommandBuffer);
        rRenderingInfo.beginRendering(rCommandBuffer);
        rRenderer.render(params);
        rRenderingInfo.endRendering(rCommandBuffer);
        rRenderingInfo.postTransition(rCommandBuffer);
        rCommandBuffer.endCommand();
    }
}  // namespace sol
