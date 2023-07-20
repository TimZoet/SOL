#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
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
    class ITask
    {
        friend class TaskGraph;

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITask() = default;

        explicit ITask(std::string name);

        ITask(const ITask&) = delete;

        ITask(ITask&&) noexcept = default;

        virtual ~ITask() noexcept = default;

        ITask& operator=(const ITask&) = delete;

        ITask& operator=(ITask&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] const std::string& getName() const noexcept;

        [[nodiscard]] TaskGraph& getTaskGraph() noexcept;

        [[nodiscard]] const TaskGraph& getTaskGraph() const noexcept;

        [[nodiscard]] const std::vector<ITask*>& getDependencies() const noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void setName(std::string value);

        template<std::derived_from<ITask> T, std::derived_from<ITask>... Ts>
        void addDependency(T& task, Ts&... tasks)
        {
            (addDependencyImpl(task), ..., addDependencyImpl(tasks));
        }

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        virtual void finalize() = 0;

        virtual void operator()() = 0;

    protected:
        void addDependencyImpl(ITask& task);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::string name;

        TaskGraph* taskGraph = nullptr;

        size_t index = 0;

        std::vector<ITask*> dependencies;
    };
}  // namespace sol