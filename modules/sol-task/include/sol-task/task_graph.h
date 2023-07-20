#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <atomic>
#include <barrier>
#include <functional>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"

namespace sol
{
    class TaskGraph
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct TaskNode
        {
            /**
             * \brief Task to be executed by this node.
             */
            ITask* task = nullptr;

            /**
             * \brief Counter decremented by each node on which this node depends.
             */
            std::unique_ptr<std::atomic_uint32_t> wait;

            /**
             * \brief Indices of nodes that depend on this node.
             */
            std::vector<size_t> notify;
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TaskGraph() = delete;

        explicit TaskGraph(VulkanDevice& dev);

        TaskGraph(const TaskGraph&) = delete;

        TaskGraph(TaskGraph&&) = delete;

        ~TaskGraph() noexcept;

        TaskGraph& operator=(const TaskGraph&) = delete;

        TaskGraph& operator=(TaskGraph&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] bool isFinalized() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Graph setup.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Directly construct a new task of the specified type.
         * \tparam T Task type.
         * \tparam Args Task constructor parameter types.
         * \param args Task constructor parameters.
         * \return Reference to new task.
         */
        template<std::derived_from<ITask> T, typename... Args>
        T& createTask(Args&&... args)
        {
            auto  task    = std::make_unique<T>(std::forward<Args>(args)...);
            auto& taskRef = *task;
            tasks.emplace_back(std::move(task));
            taskRef.taskGraph = this;
            return taskRef;
        }

        /**
         * \brief Add task to graph.
         * \tparam T Task type.
         * \param task Task.
         * \return Reference to task.
         */
        template<std::derived_from<ITask> T>
        T& addTask(std::unique_ptr<T> task)
        {
            auto& taskRef = *task;
            tasks.emplace_back(std::move(task));
            taskRef.taskGraph = this;
            return taskRef;
        }

        void finalize();

        void requireFinalized() const;

        void requireNonFinalized() const;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Start a new iteration of the task graph.
         * Must be called exactly once before calling getNext() and end().
         */
        void start();

        /**
         * \brief Get next available task. The return value being empty does not mean all tasks have finished yet.
         * There may be unscheduled tasks still waiting on their dependencies to complete. Caller may choose to
         * keep calling getNext, yield, or do something else entirely.
         * \return Callable that runs task.
         */
        std::optional<std::function<void()>> getNext();

        /**
         * \brief Wait for all tasks to be completed. Blocking call.
         * Must be called exactly once to ensure all tasks have completed and before next iteration kicked of by start().
         */
        void end();

    private:
        void makeAvailable(size_t index) noexcept;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        std::vector<ITaskPtr> tasks;

        std::vector<TaskNode> nodes;

        std::vector<size_t> available;

        std::mutex mutex;

        size_t finalNodeCount = 0;

        /**
         * \brief Barrier that is notified by final node and from the end method.
         */
        std::barrier<> finished;
    };
}  // namespace sol
