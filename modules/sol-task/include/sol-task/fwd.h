#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class TaskGraph;
    class ITask;
    template<typename T>
    class ITaskResource;
    template<typename T>
    class ITaskResourceList;

    using TaskGraphPtr = std::unique_ptr<TaskGraph>;
    using ITaskPtr     = std::unique_ptr<ITask>;
    template<typename T>
    using ITaskResourcePtr = std::unique_ptr<ITaskResource<T>>;
    template<typename T>
    using ITaskResourceListPtr = std::unique_ptr<ITaskResourceList<T>>;
}  // namespace sol
