#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/resources/i_task_resource.h"

namespace sol
{
    class CommandBufferResource final : public ITaskResource
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Usage
        {
            /**
             * \brief This task writes to (records) the command buffer.
             */
            Record = 1,

            /**
             * \brief This task submits the command buffer.
             */
            Submit = 2,

            /**
             * \brief This task depends on the results of the command buffer.
             */
            Await = 4
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        CommandBufferResource() = delete;

        explicit CommandBufferResource(TaskGraph& taskGraph);

        CommandBufferResource(const CommandBufferResource&) = delete;

        CommandBufferResource(CommandBufferResource&&) = delete;

        ~CommandBufferResource() noexcept override;

        CommandBufferResource& operator=(const CommandBufferResource&) = delete;

        CommandBufferResource& operator=(CommandBufferResource&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] ITask& getSubmitter() const;

        [[nodiscard]] const std::vector<ITask*>& getWaiters() const noexcept;

        [[nodiscard]] uint32_t getCount() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void addUsage(ITask& task, Usage usage);

        void setCount(uint32_t c);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITask* submitter = nullptr;

        std::vector<ITask*> waiters;

        uint32_t count = 0;
    };
}  // namespace sol
