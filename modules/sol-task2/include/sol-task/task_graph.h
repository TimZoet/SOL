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

            uint32_t waitCount = 0;

            /**
             * \brief Nodes that depend on this node.
             */
            std::vector<TaskNode*> notify;
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TaskGraph() = delete;

        TaskGraph(VulkanDevice& dev, VulkanCommandPool& cPool);

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

        ////////////////////////////////////////////////////////////////
        // Graph setup.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Construct a new task of the specified type.
         * \tparam T Task type.
         * \tparam Args Additional constructor parameter types.
         * \param args Additional constructor parameters.
         * \return Reference to new task.
         */
        template<std::derived_from<ITask> T, typename... Args>
        [[nodiscard]] T& createTask(Args&&... args)
        {
            auto  task    = std::make_unique<T>(*this, std::forward<Args>(args)...);
            auto& taskRef = *task;
            tasks.emplace_back(std::move(task));
            return taskRef;
        }

        [[nodiscard]] CommandBufferResource& createCommandBuffer();

        [[nodiscard]] IndexResource& createIndex(uint32_t range);

        [[nodiscard]] CompiledGraphPtr compile() const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        VulkanCommandPool* commandPool = nullptr;

        std::vector<ITaskPtr> tasks;

        struct
        {
            std::vector<IndexResourcePtr> indices;

            std::vector<CommandBufferResourcePtr> commandBuffers;
        } resources;

    };
}  // namespace sol
