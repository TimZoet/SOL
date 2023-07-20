#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-render/graphics/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task.h"

namespace sol
{
    class GraphicsRenderTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsRenderTask();

        /**
         * \brief GraphicsRenderTask.
         * \param taskName User-friendly name of task.
         * \param rd Renderer with which commands are recorded.
         * \param rdData Render data describing commands to be recorded.
         * \param rdInfo Rendering info with which the begin/end rendering commands are called.
         * \param cmdBuffer Command buffer to record to.
         * \param frameIdx Index passed to the renderer for selecting the descriptor sets that are bound.
         */
        GraphicsRenderTask(std::string                           taskName,
                           ITaskResource<GraphicsRenderer>*      rd,
                           ITaskResource<GraphicsRenderData>*    rdData,
                           ITaskResource<GraphicsRenderingInfo>* rdInfo,
                           ITaskResource<VulkanCommandBuffer>*   cmdBuffer,
                           ITaskResource<uint32_t>*              frameIdx);

        GraphicsRenderTask(const GraphicsRenderTask&) = delete;

        GraphicsRenderTask(GraphicsRenderTask&&) noexcept;

        ~GraphicsRenderTask() noexcept override;

        GraphicsRenderTask& operator=(const GraphicsRenderTask&) = delete;

        GraphicsRenderTask& operator=(GraphicsRenderTask&&) noexcept;

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
        ITaskResource<GraphicsRenderer>* renderer = nullptr;

        /**
         * \brief Render data describing commands to be recorded.
         */
        ITaskResource<GraphicsRenderData>* renderData = nullptr;

        /**
         * \brief Rendering info with which the begin/end rendering commands are called.
         */
        ITaskResource<GraphicsRenderingInfo>* renderingInfo = nullptr;

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
