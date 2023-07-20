#include "sol-task/other/fence_task.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource_list.h"
#include "sol-task/task_graph.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    FenceTask::FenceTask() = default;

    FenceTask::FenceTask(std::string taskName, const Action a, ITaskResourceList<VulkanFence>* f) :
        ITask(std::move(taskName)), action(a), fences(f)
    {
    }

    FenceTask::FenceTask(FenceTask&&) noexcept = default;

    FenceTask::~FenceTask() noexcept = default;

    FenceTask& FenceTask::operator=(FenceTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void FenceTask::finalize()
    {
        if (!fences) throw SolError("Cannot finalize FenceTask: fences not set.");
    }

    void FenceTask::operator()()
    {
        // Get handles.
        const auto fenceHandles = *fences | std::views::transform([](const VulkanFence& f) { return f.get(); }) |
                                  std::ranges::to<std::vector>();

        if (fenceHandles.empty()) return;

        // Wait for fences.
        if (any(action & Action::Wait))
            handleVulkanError(vkWaitForFences(getTaskGraph().getDevice().get(),
                                              static_cast<uint32_t>(fenceHandles.size()),
                                              fenceHandles.data(),
                                              VK_TRUE,
                                              UINT64_MAX));

        // Reset fences.
        if (any(action & Action::Reset))
            handleVulkanError(vkResetFences(
              getTaskGraph().getDevice().get(), static_cast<uint32_t>(fenceHandles.size()), fenceHandles.data()));
    }
}  // namespace sol
