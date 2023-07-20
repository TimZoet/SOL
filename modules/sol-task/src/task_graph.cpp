#include "sol-task/task_graph.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <functional>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TaskGraph::TaskGraph(VulkanDevice& dev) : device(&dev), finished(2) {}

    TaskGraph::~TaskGraph() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& TaskGraph::getDevice() noexcept { return *device; }

    const VulkanDevice& TaskGraph::getDevice() const noexcept { return *device; }

    bool TaskGraph::isFinalized() const noexcept { return !nodes.empty(); }

    ////////////////////////////////////////////////////////////////
    // Graph setup.
    ////////////////////////////////////////////////////////////////

    void TaskGraph::finalize()
    {
        // TODO: Check for cycles.

        requireNonFinalized();
        if (tasks.empty()) throw SolError("Cannot finalize empty TaskGraph.");

        for (const auto& task : tasks) task->finalize();

        // Create a node for each task with a counter that will get decremented by dependencies.
        for (auto& task : tasks)
        {
            task->index = nodes.size();
            nodes.emplace_back(TaskNode{task.get(), std::make_unique<std::atomic_uint32_t>(), {}});
        }

        // Add notifications.
        for (const auto& task : tasks)
        {
            for (auto& dep : task->getDependencies()) nodes[dep->index].notify.emplace_back(task->index);
        }

        // Count the number of nodes without dependents and create final node that waits on all of these nodes for ending the graph.
        finalNodeCount = std::ranges::count_if(nodes, [](const TaskNode& item) { return item.notify.empty(); });
        nodes.emplace_back(nullptr, std::make_unique<std::atomic_uint32_t>());

        // Make nodes without dependents notify final node.
        for (auto& [task, wait, notify] :
             nodes | std::views::filter([](const TaskNode& item) { return item.notify.empty(); }))
        {
            notify.emplace_back(nodes.size() - 1);
        }
    }

    void TaskGraph::requireFinalized() const
    {
        if (!isFinalized()) throw SolError("TaskGraph was not yet finalized.");
    }

    void TaskGraph::requireNonFinalized() const
    {
        if (isFinalized()) throw SolError("TaskGraph was already finalized.");
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void TaskGraph::start()
    {
        requireFinalized();

        // Reset counters and make all nodes without dependencies available.
        for (auto& [task, wait, notify] : nodes)
        {
            // This is the final, taskless node.
            if (!task)
            {
                *wait = static_cast<uint32_t>(finalNodeCount);
                continue;
            }

            *wait = static_cast<uint32_t>(task->getDependencies().size());

            // Make node without dependencies available.
            if (task->getDependencies().empty()) available.emplace_back(task->index);
        }
    }

    std::optional<std::function<void()>> TaskGraph::getNext()
    {
        requireFinalized();

        std::scoped_lock lock(mutex);

        if (available.empty()) { return {}; }

        const auto index = available.back();
        available.pop_back();

        return [index, this] {
            const auto& [task, _, notify] = nodes[index];

            // Run task.
            if (task) task->operator()();
            // This is the final node.
            else
            {
                finished.arrive_and_wait();
                return;
            }

            // Notify dependencies of completion. If counter reaches 0, make node available.
            for (const auto i : notify)
            {
                if (--*nodes[i].wait == 0) makeAvailable(i);
            }
        };
    }

    void TaskGraph::end()
    {
        requireFinalized();

        // Wait on final node.
        finished.arrive_and_wait();
    }

    void TaskGraph::makeAvailable(const size_t index) noexcept
    {
        std::scoped_lock lock(mutex);
        available.emplace_back(index);
    }
}  // namespace sol
