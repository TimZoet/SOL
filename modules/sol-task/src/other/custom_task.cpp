#include "sol-task/other/custom_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    CustomTask::CustomTask() = default;

    CustomTask::CustomTask(std::string taskName, std::function<void()> f) :
        ITask(std::move(taskName)), function(std::move(f))
    {
    }

    CustomTask::CustomTask(CustomTask&&) noexcept = default;

    CustomTask::~CustomTask() noexcept = default;

    CustomTask& CustomTask::operator=(CustomTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void CustomTask::finalize()
    {
        if (!function) throw SolError("Cannot finalize CustomTask: no function set.");
    }

    void CustomTask::operator()() { function(); }
}  // namespace sol
