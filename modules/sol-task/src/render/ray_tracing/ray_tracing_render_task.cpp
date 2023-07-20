#include "sol-task/render/ray_tracing/ray_tracing_render_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_frame_buffer.h"
#include "sol-error/sol_error.h"
#include "sol-render/ray_tracing/ray_tracing_renderer.h"


////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingRenderTask::RayTracingRenderTask() = default;

    RayTracingRenderTask::RayTracingRenderTask(std::string                          taskName,
                                               ITaskResource<RayTracingRenderer>*   rd,
                                               ITaskResource<RayTracingRenderData>* rdData,
                                               ITaskResource<VulkanCommandBuffer>*  cmdBuffer,
                                               ITaskResource<uint32_t>*             frameIdx) :
        ITask(std::move(taskName)), renderer(rd), renderData(rdData), commandBuffer(cmdBuffer), frameIndex(frameIdx)
    {
    }

    RayTracingRenderTask::RayTracingRenderTask(RayTracingRenderTask&&) noexcept = default;

    RayTracingRenderTask::~RayTracingRenderTask() noexcept = default;

    RayTracingRenderTask& RayTracingRenderTask::operator=(RayTracingRenderTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void RayTracingRenderTask::finalize()
    {
        if (!renderer) throw SolError("Cannot finalize RayTracingRenderTask: renderer not set.");
        if (!renderData) throw SolError("Cannot finalize RayTracingRenderTask: renderData not set.");
        if (!commandBuffer) throw SolError("Cannot finalize RayTracingRenderTask: commandBuffer not set.");
        if (!frameIndex) throw SolError("Cannot finalize RayTracingRenderTask: frameIndex not set.");
    }

    void RayTracingRenderTask::operator()()
    {
        auto&       rRenderer      = **renderer;
        const auto& rRenderData    = **renderData;
        const auto& rCommandBuffer = **commandBuffer;
        const auto  rFrameIndex    = **frameIndex;

        rCommandBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        rCommandBuffer.beginOneTimeCommand();
        const RayTracingRenderer::Parameters params = {
          .renderData = rRenderData, .commandBuffer = rCommandBuffer.get(), .index = rFrameIndex};
        rRenderer.createPipelines(params);
        rRenderer.render(params);
        rCommandBuffer.endCommand();
    }
}  // namespace sol
