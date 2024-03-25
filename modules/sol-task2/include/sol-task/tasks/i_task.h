#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <unordered_map>
#include <vector>

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
            SignalSemaphore
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

        [[nodiscard]] const std::vector<ITask*>& getAwaits() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setName(std::string n);

        void addDependency(ITask& t);

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

        void addAwait(ITask& t);

        void addAwait(CommandBufferResource& res);

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

        std::vector<ITask*> dependencies;

        std::vector<ITask*> awaits;
    };
}  // namespace sol
