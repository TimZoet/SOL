#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"

namespace sol
{
    class ITaskResource
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITaskResource() = delete;

        explicit ITaskResource(TaskGraph& taskGraph);

        ITaskResource(const ITaskResource&) = delete;

        ITaskResource(ITaskResource&&) = delete;

        virtual ~ITaskResource() noexcept;

        ITaskResource& operator=(const ITaskResource&) = delete;

        ITaskResource& operator=(ITaskResource&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] TaskGraph& getTaskGraph() noexcept;

        [[nodiscard]] const TaskGraph& getTaskGraph() const noexcept;

        [[nodiscard]] ITask* getWriter() const noexcept;

        [[nodiscard]] const std::vector<ITask*>& getReaders() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setWriter(ITask& task);

        void addReader(ITask& task);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        TaskGraph* graph = nullptr;

        ITask* writer = nullptr;

        std::vector<ITask*> readers;
    };
}  // namespace sol
