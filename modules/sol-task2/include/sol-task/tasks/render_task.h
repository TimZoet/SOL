#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/tasks/i_task.h"

namespace sol
{
    class RenderTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RenderTask() = delete;

        explicit RenderTask(TaskGraph& taskGraph);

        RenderTask(const RenderTask&) = delete;

        RenderTask(RenderTask&&) = delete;

        ~RenderTask() noexcept override;

        RenderTask& operator=(const RenderTask&) = delete;

        RenderTask& operator=(RenderTask&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the command buffer resource this task records to.
         * \return CommandBufferResource.
         */
        [[nodiscard]] CommandBufferResource& getCommandBuffer() const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the number of command buffers used for double, triple or more buffering.
         * \param count Count.
         */
        void setBufferCount(uint32_t count);

        ////////////////////////////////////////////////////////////////
        // Compile.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::function<void()>
          compile(const CompiledGraph::Node&                            node,
                  std::unordered_map<const ITaskResource*, IProvider*>& providerLookup) override;

        // TODO: Deprecate.
        std::function<void(VulkanCommandBuffer&, const uint32_t)> tmpFunction;
        const IndexResource* tmpIndex = nullptr;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        CommandBufferResource* commandBuffer = nullptr;
    };
}  // namespace sol
