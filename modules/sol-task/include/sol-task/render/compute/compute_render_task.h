#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-render/compute/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task.h"

namespace sol
{
    class ComputeRenderTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeRenderTask();

        ComputeRenderTask(std::string                         taskName,
                          ITaskResource<ComputeRenderer>*     rd,
                          ITaskResource<ComputeRenderData>*   rdData,
                          ITaskResource<VulkanCommandBuffer>* cmdBuffer,
                          ITaskResource<uint32_t>*            frameIdx);

        ComputeRenderTask(const ComputeRenderTask&) = delete;

        ComputeRenderTask(ComputeRenderTask&&) noexcept;

        ~ComputeRenderTask() noexcept override;

        ComputeRenderTask& operator=(const ComputeRenderTask&) = delete;

        ComputeRenderTask& operator=(ComputeRenderTask&&) noexcept;

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
        ITaskResource<ComputeRenderer>* renderer = nullptr;

        /**
         * \brief Render data describing commands to be recorded.
         */
        ITaskResource<ComputeRenderData>* renderData = nullptr;

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
