#include "sol-task/render/compute/compute_render_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_frame_buffer.h"
#include "sol-error/sol_error.h"
#include "sol-render/compute/compute_renderer.h"


////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeRenderTask::ComputeRenderTask() = default;

    ComputeRenderTask::ComputeRenderTask(std::string                         taskName,
                                         ITaskResource<ComputeRenderer>*     rd,
                                         ITaskResource<ComputeRenderData>*   rdData,
                                         ITaskResource<VulkanCommandBuffer>* cmdBuffer,
                                         ITaskResource<uint32_t>*            frameIdx) :
        ITask(std::move(taskName)), renderer(rd), renderData(rdData), commandBuffer(cmdBuffer), frameIndex(frameIdx)
    {
    }

    ComputeRenderTask::ComputeRenderTask(ComputeRenderTask&&) noexcept = default;

    ComputeRenderTask::~ComputeRenderTask() noexcept = default;

    ComputeRenderTask& ComputeRenderTask::operator=(ComputeRenderTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void ComputeRenderTask::finalize()
    {
        if (!renderer) throw SolError("Cannot finalize ComputeRenderTask: renderer not set.");
        if (!renderData) throw SolError("Cannot finalize ComputeRenderTask: renderData not set.");
        if (!commandBuffer) throw SolError("Cannot finalize ComputeRenderTask: commandBuffer not set.");
        if (!frameIndex) throw SolError("Cannot finalize ComputeRenderTask: frameIndex not set.");
    }

    void ComputeRenderTask::operator()()
    {
        auto&       rRenderer      = **renderer;
        const auto& rRenderData    = **renderData;
        const auto& rCommandBuffer = **commandBuffer;
        const auto  rFrameIndex    = **frameIndex;

        rCommandBuffer.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        rCommandBuffer.beginOneTimeCommand();
        const ComputeRenderer::Parameters params = {
          .renderData = rRenderData, .commandBuffer = rCommandBuffer.get(), .index = rFrameIndex};
        rRenderer.createPipelines(params);
        rRenderer.render(params);
        rCommandBuffer.endCommand();
    }
}  // namespace sol
