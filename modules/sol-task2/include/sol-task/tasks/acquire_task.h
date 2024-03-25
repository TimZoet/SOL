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
    class AcquireTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        AcquireTask() = delete;

        explicit AcquireTask(TaskGraph& taskGraph);

        AcquireTask(const AcquireTask&) = delete;

        AcquireTask(AcquireTask&&) = delete;

        ~AcquireTask() noexcept override;

        AcquireTask& operator=(const AcquireTask&) = delete;

        AcquireTask& operator=(AcquireTask&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool supportsCapability(Capability capability) const noexcept override;

        [[nodiscard]] IndexResource& getImageIndex() const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setQueue(VulkanQueue& q);

        void setSwapchain(VulkanSwapchain& chain);

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
