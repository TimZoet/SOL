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

    const std::vector<ITask*>& CommandBufferResource::getWaiters() const noexcept { return waiters; }

    uint32_t CommandBufferResource::getCount() const noexcept { return count; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void CommandBufferResource::addUsage(ITask& task, const Usage usage)
    {
        // TODO: Once timeline semaphores are used, the Await does not need to be added as reader anymore.
        // submitter can also be deprecated, since there will be only one read.
        switch (usage)
        {
        case Usage::Record: setWriter(task); break;
        case Usage::Submit:
            if (submitter) throw SolError("Cannot set submitting task. It was already set.");
            addReader(task);
            submitter = &task;
            break;
        case Usage::Await:
            addReader(task);
            waiters.push_back(&task);
            break;
        }
    }

    void CommandBufferResource::setCount(const uint32_t c) { count = c; }

}  // namespace sol
