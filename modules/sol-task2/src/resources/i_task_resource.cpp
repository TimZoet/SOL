#include "sol-task/resources/i_task_resource.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ITaskResource::ITaskResource(TaskGraph& taskGraph) : graph(&taskGraph) {}

    ITaskResource::~ITaskResource() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    TaskGraph& ITaskResource::getTaskGraph() noexcept { return *graph; }

    const TaskGraph& ITaskResource::getTaskGraph() const noexcept { return *graph; }

    ITask* ITaskResource::getWriter() const noexcept { return writer; }

    const std::vector<ITask*>& ITaskResource::getReaders() const noexcept { return readers; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ITaskResource::setWriter(ITask& task)
    {
        if (writer) throw SolError("Writer was already set.");
        writer = &task;
    }

    void ITaskResource::addReader(ITask& task) { readers.push_back(&task); }

}  // namespace sol
