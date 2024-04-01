#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <atomic>
#include <barrier>
#include <functional>
#include <mutex>
#include <optional>
#include <set>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

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
    class CompiledGraph
    {
        friend class TaskGraph;

    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Node
        {
            std::string name;

            /**
             * \brief Function to run.
             */
            std::optional<std::function<void()>> f;

            /**
             * \brief Execution dependencies that must be waited on.
             */
            std::set<Node*> dependencies;

            /**
             * \brief Execution dependents that must be notified.
             */
            std::set<Node*> dependents;

            /**
             * \brief Semaphores that must be signaled.
             */
            std::vector<SemaphoreProvider*> signalSemaphores;

            /**
             * \brief Semaphores that must be awaited.
             */
            std::vector<std::pair<SemaphoreProvider*, VkPipelineStageFlags>> awaitSemaphores;

            /**
             * \brief Semaphores that must be signaled.
             */
            std::vector<TimelineSemaphoreProvider*> signalTimelineSemaphores;

            /**
             * \brief Semaphores that must be awaited.
             */
            std::vector<std::pair<TimelineSemaphoreProvider*, VkPipelineStageFlags>> awaitTimelineSemaphores;

            /**
             * \brief Counter decremented by each node on which this node depends.
             */
            std::atomic_uint32_t wait;

            [[nodiscard]] std::pair<std::vector<VkSemaphore>, std::vector<VkPipelineStageFlags>>
              getAwaitSemaphores() const;

            [[nodiscard]] std::vector<VkSemaphore> getSignalSemaphores() const;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        CompiledGraph() = delete;

        CompiledGraph(VulkanDevice& dev, VulkanCommandPool& cPool);

        CompiledGraph(const CompiledGraph&) = delete;

        CompiledGraph(CompiledGraph&&) = delete;

        ~CompiledGraph() noexcept;

        CompiledGraph& operator=(const CompiledGraph&) = delete;

        CompiledGraph& operator=(CompiledGraph&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] VulkanCommandPool& getCommandPool() noexcept;

        [[nodiscard]] const VulkanCommandPool& getCommandPool() const noexcept;

        [[nodiscard]] bool isEnded() const noexcept;

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
        void makeAvailable(Node& node) noexcept;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanDevice* device = nullptr;

        VulkanCommandPool* commandPool = nullptr;

        std::vector<std::unique_ptr<Node>> nodes;

        struct
        {
            std::vector<IndexProviderPtr> indices;

            std::vector<CommandBufferProviderPtr> commandBuffers;

            std::vector<SemaphoreProviderPtr> semaphores;

            std::vector<TimelineSemaphoreProviderPtr> timelineSemaphores;
        } resources;

        std::vector<Node*> available;

        std::mutex mutex;

        /**
         * \brief Barrier that is notified by final node and from the end method.
         */
        std::barrier<> finished;
    };
}  // namespace sol
