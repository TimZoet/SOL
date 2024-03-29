#pragma once

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
    class PresentTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PresentTask() = delete;

        explicit PresentTask(TaskGraph& taskGraph);

        PresentTask(const PresentTask&) = delete;

        PresentTask(PresentTask&&) = delete;

        ~PresentTask() noexcept override;

        PresentTask& operator=(const PresentTask&) = delete;

        PresentTask& operator=(PresentTask&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool supportsCapability(Capability) const noexcept override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setQueue(VulkanQueue& q);

        void setSwapchain(VulkanSwapchain& chain, IndexResource& i);

        ////////////////////////////////////////////////////////////////
        // Compile.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::function<void()>
          compile(const CompiledGraph::Node&                            node,
                  std::unordered_map<const ITaskResource*, IProvider*>& providerLookup) override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanQueue* queue = nullptr;

        VulkanSwapchain* swapchain = nullptr;

        IndexResource* index = nullptr;
    };
}  // namespace sol
