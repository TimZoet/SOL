#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-render/ray_tracing/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task.h"

namespace sol
{
    class RayTracingRenderTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingRenderTask();

        RayTracingRenderTask(std::string                          taskName,
                             ITaskResource<RayTracingRenderer>*   rd,
                             ITaskResource<RayTracingRenderData>* rdData,
                             ITaskResource<VulkanCommandBuffer>*  cmdBuffer,
                             ITaskResource<uint32_t>*             frameIdx);

        RayTracingRenderTask(const RayTracingRenderTask&) = delete;

        RayTracingRenderTask(RayTracingRenderTask&&) noexcept;

        ~RayTracingRenderTask() noexcept override;

        RayTracingRenderTask& operator=(const RayTracingRenderTask&) = delete;

        RayTracingRenderTask& operator=(RayTracingRenderTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Renderer with which commands are recorded.
         */
        ITaskResource<RayTracingRenderer>* renderer = nullptr;

        /**
         * \brief Render data describing commands to be recorded.
         */
        ITaskResource<RayTracingRenderData>* renderData = nullptr;

        /**
         * \brief Command buffer to record to.
         */
        ITaskResource<VulkanCommandBuffer>* commandBuffer = nullptr;

        /**
         * \brief Index passed to the renderer for selecting the descriptor sets that are bound.
         */
        ITaskResource<uint32_t>* frameIndex = nullptr;
    };
}  // namespace sol
