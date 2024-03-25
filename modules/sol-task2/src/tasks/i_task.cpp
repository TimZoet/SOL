#include "sol-task/tasks/i_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/task_graph.h"
#include "sol-task/resources/command_buffer_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ITask::ITask(TaskGraph& taskGraph) : graph(&taskGraph) {}

    ITask::~ITask() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    TaskGraph& ITask::getTaskGraph() noexcept { return *graph; }

    const TaskGraph& ITask::getTaskGraph() const noexcept { return *graph; }

    const std::string& ITask::getName() const noexcept { return name; }

    bool ITask::supportsCapability(Capability) const noexcept { return false; }

    const std::vector<ITask*>& ITask::getDependencies() const noexcept { return dependencies; }

    const std::vector<ITask*>& ITask::getAwaits() const noexcept { return awaits; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ITask::setName(std::string n) { name = std::move(n); }

    void ITask::addDependency(ITask& t) { dependencies.push_back(&t); }

    void ITask::addWrite(ITaskResource& res) { res.setWriter(*this); }

    void ITask::addRead(ITaskResource& res) { res.addReader(*this); }

    void ITask::addAwait(ITask& t)
    {
        if (!supportsCapability(Capability::AwaitSemaphore))
            throw SolError("Capability::AwaitSemaphore is not supported.");
        if (!t.supportsCapability(Capability::SignalSemaphore))
            throw SolError("Capability::SignalSemaphore is not supported.");
        awaits.push_back(&t);
        // TODO: If signal can be done using counting semaphore, execution dependency is no longer needed.
        addDependency(t);
    }

    void ITask::addAwait(CommandBufferResource& res)
    {
        if (!supportsCapability(Capability::AwaitSemaphore))
            throw SolError("Capability::AwaitSemaphore is not supported.");
        res.addUsage(*this, CommandBufferResource::Usage::Await);
    }
}  // namespace sol
