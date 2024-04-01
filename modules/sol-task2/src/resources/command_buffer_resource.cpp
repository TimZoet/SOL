#include "sol-task/resources/command_buffer_resource.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    CommandBufferResource::CommandBufferResource(TaskGraph& taskGraph) : ITaskResource(taskGraph) {}

    CommandBufferResource::~CommandBufferResource() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    ITask& CommandBufferResource::getSubmitter() const
    {
        if (!submitter) throw SolError("Cannot get submitting task. It was not set yet.");
        return *submitter;
    }

    const std::vector<std::pair<ITask*, VkPipelineStageFlags>>& CommandBufferResource::getWaiters() const noexcept
    {
        return waiters;
    }

    uint32_t CommandBufferResource::getCount() const noexcept { return count; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void CommandBufferResource::setRecorder(ITask& task) { setWriter(task); }

    void CommandBufferResource::setSubmitter(ITask& task)
    {
        if (submitter) throw SolError("Cannot set submitting task. It was already set.");
        // TODO: Once timeline semaphores are used, waits and signals can be done in arbitrary order.
        // The task does not need to be added as reader anymore then.
        addReader(task);
        submitter = &task;
    }

    void CommandBufferResource::addAwait(ITask& task, const VkPipelineStageFlags stages)
    {
        // TODO: Once timeline semaphores are used, waits and signals can be done in arbitrary order.
        // The task does not need to be added as reader anymore then.
        addReader(task);
        waiters.emplace_back(&task, stages);
    }

    void CommandBufferResource::setCount(const uint32_t c) { count = c; }
}  // namespace sol
