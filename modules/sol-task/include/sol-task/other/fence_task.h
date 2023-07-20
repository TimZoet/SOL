#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/i_task.h"

namespace sol
{
    class FenceTask final : public ITask
    {
    public:
        enum class Action
        {
            Wait  = 1,
            Reset = 2
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        FenceTask();

        FenceTask(std::string taskName, Action a, ITaskResourceList<VulkanFence>* f);

        FenceTask(const FenceTask&) = delete;

        FenceTask(FenceTask&&) noexcept;

        ~FenceTask() noexcept override;

        FenceTask& operator=(const FenceTask&) = delete;

        FenceTask& operator=(FenceTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        Action action = Action::Wait;

        ITaskResourceList<VulkanFence>* fences = nullptr;
    };
}  // namespace sol
