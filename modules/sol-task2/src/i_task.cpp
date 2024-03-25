#include "sol-task/i_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-task/task_graph.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ITask::ITask(std::string name) : name(std::move(name)) {}

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& ITask::getDevice() noexcept { return taskGraph->getDevice(); }

    const VulkanDevice& ITask::getDevice() const noexcept { return taskGraph->getDevice(); }

    const std::string& ITask::getName() const noexcept { return name; }

    TaskGraph& ITask::getTaskGraph() noexcept { return *taskGraph; }

    const TaskGraph& ITask::getTaskGraph() const noexcept { return *taskGraph; }

    const std::vector<ITask*>& ITask::getDependencies() const noexcept { return dependencies; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void ITask::setName(std::string value) { name = std::move(value); }

    void ITask::addDependencyImpl(ITask& task)
    {
        if (task.taskGraph != taskGraph) throw SolError("Cannot add dependency, tasks are not in same graph.");
        dependencies.push_back(&task);
    }
}  // namespace sol
