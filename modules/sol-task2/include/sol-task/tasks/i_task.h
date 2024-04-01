#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <source_location>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////


#include "sol-task/compiled_graph.h"
#include "sol-task/fwd.h"

namespace sol
{
    class ITask
    {
    public:
        friend class TaskGraph;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Capability
        {
            AwaitSemaphore,
            SignalSemaphore,
            TimelineSemaphore
        };

        struct SemaphoreAwait
        {
            ITask*               src;
            VkPipelineStageFlags flags;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITask() = delete;

        explicit ITask(TaskGraph& taskGraph);

        ITask(const ITask&) = delete;

        ITask(ITask&&) = delete;

        virtual ~ITask() noexcept;

        ITask& operator=(const ITask&) = delete;

        ITask& operator=(ITask&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] TaskGraph& getTaskGraph() noexcept;

        [[nodiscard]] const TaskGraph& getTaskGraph() const noexcept;

        [[nodiscard]] const std::string& getName() const noexcept;

        [[nodiscard]] virtual bool supportsCapability(Capability capability) const noexcept;

        [[nodiscard]] const std::vector<ITask*>& getDependencies() const noexcept;

        [[nodiscard]] const std::vector<SemaphoreAwait>& getAwaits() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setName(std::string n);

        /**
         * \brief Add a CPU-side execution dependency to the given source task. This task will not run before the other has finished.
         * \param src Source task to wait on.
         */
        void addDependency(ITask& src);

        /**
         * \brief This task will write to the given resource. Used to resolve CPU-side execution dependencies.
         * \param res Resource.
         */
        void addWrite(ITaskResource& res);

        /**
         * \brief This task will read from the given resource. Used to resolve CPU-side execution dependencies.
         * \param res Resource.
         */
        void addRead(ITaskResource& res);

        /**
         * \brief Have the work submitted by this task wait for the given source task using a semaphore. Must support Capability::AwaitSemaphore.
         * \param src Source task to await. Must support Capability::SignalSemaphore.
         * \param stages Destination stages.
         */
        void addAwait(ITask& src, VkPipelineStageFlags stages);

        /**
         * \brief Have the work submitted by this task wait for the completion of the given command buffer. Must support Capability::AwaitSemaphore.
         * \param src Command buffer to await.
         * \param stages Destination stages.
         */
        void addAwait(CommandBufferResource& src, VkPipelineStageFlags stages);

        ////////////////////////////////////////////////////////////////
        // Compile.
        ////////////////////////////////////////////////////////////////

        // TODO: Lookup should be constant.
        [[nodiscard]] virtual std::function<void()>
          compile(const CompiledGraph::Node&                            node,
                  std::unordered_map<const ITaskResource*, IProvider*>& providerLookup) = 0;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        TaskGraph* graph = nullptr;

        std::string name;

        std::source_location source;

        std::vector<ITask*> dependencies;

        std::vector<SemaphoreAwait> awaits;
    };
}  // namespace sol
