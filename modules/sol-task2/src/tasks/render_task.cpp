#include "sol-task/tasks/render_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/task_graph.h"
#include "sol-task/providers/command_buffer_provider.h"
#include "sol-task/resources/command_buffer_resource.h"
#include "sol-task/resources/index_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RenderTask::RenderTask(TaskGraph& taskGraph) : ITask(taskGraph) {}

    RenderTask::~RenderTask() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    CommandBufferResource& RenderTask::getCommandBuffer() const
    {
        if (!commandBuffer) throw SolError("Cannot get command buffer. It was not created yet.");
        return *commandBuffer;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RenderTask::setBufferCount(const uint32_t count)
    {
        if (commandBuffer) throw SolError("Cannot set buffer count. It was already set.");
        commandBuffer = &getTaskGraph().createCommandBuffer();
        commandBuffer->addUsage(*this, CommandBufferResource::Usage::Record);
        commandBuffer->setCount(count);
    }

    ////////////////////////////////////////////////////////////////
    // Compile.
    ////////////////////////////////////////////////////////////////

    std::function<void()> RenderTask::compile(const CompiledGraph::Node&,
                                              std::unordered_map<const ITaskResource*, IProvider*>& providerLookup)
    {
        auto func = [commandBufferProvider = static_cast<CommandBufferProvider*>(providerLookup[commandBuffer]),
                     indexProvider         = static_cast<IndexProvider*>(providerLookup[tmpIndex]),
                     func = tmpFunction] { func(commandBufferProvider->get(), indexProvider->getValue()); };

        return func;
    }
}  // namespace sol
