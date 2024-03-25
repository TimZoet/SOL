#include "sol-task/tasks/submit_task.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/providers/command_buffer_provider.h"
#include "sol-task/resources/command_buffer_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    SubmitTask::SubmitTask(TaskGraph& taskGraph) : ITask(taskGraph) {}

    SubmitTask::~SubmitTask() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool SubmitTask::supportsCapability(const Capability capability) const noexcept
    {
        switch (capability)
        {
        case Capability::AwaitSemaphore: return true;
        default: return false;
        }
    }

    CommandBufferResource& SubmitTask::getCommandBuffer() const
    {
        if (!commandBuffer) throw SolError("Cannot get command buffer. It was not set yet.");
        return *commandBuffer;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void SubmitTask::setQueue(VulkanQueue& q) { queue = &q; }

    void SubmitTask::setCommandBuffer(CommandBufferResource& cb)
    {
        if (commandBuffer) throw SolError("Cannot set command buffer on SubmitTask. It was already set.");
        commandBuffer = &cb;
        commandBuffer->addUsage(*this, CommandBufferResource::Usage::Submit);
    }

    ////////////////////////////////////////////////////////////////
    // Compile.
    ////////////////////////////////////////////////////////////////

    std::function<void()> SubmitTask::compile(const CompiledGraph::Node&                            node,
                                              std::unordered_map<const ITaskResource*, IProvider*>& providerLookup)
    {
        auto func = [&node,
                     commandBufferProvider = static_cast<CommandBufferProvider*>(providerLookup[commandBuffer]),
                     queue                 = this->queue] {
            const auto cb = commandBufferProvider->get().get();

            const auto* fence = commandBufferProvider->getFence();

            std::vector<VkSemaphore> waitSemaphores;
            //if (commandBufferProvider->getWaitSemaphore())
            //    waitSemaphores.push_back(commandBufferProvider->getWaitSemaphore()->get());
            if (node.await) waitSemaphores.push_back(node.await->get());

            std::vector<VkSemaphore> signalSemaphores;
            if (commandBufferProvider->getSemaphore())
                signalSemaphores.push_back(commandBufferProvider->getSemaphore()->get());
            if (node.signal) signalSemaphores.push_back(node.signal->get());

            // TODO: Hardcoded flags.
            const std::array<VkPipelineStageFlags, 1> flags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

            VkSubmitInfo submitInfo{};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount   = static_cast<uint32_t>(waitSemaphores.size());
            submitInfo.pWaitSemaphores      = waitSemaphores.data();
            submitInfo.pWaitDstStageMask    = flags.data();
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &cb;
            submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
            submitInfo.pSignalSemaphores    = signalSemaphores.data();
            queue->submit(submitInfo, fence ? fence->get() : VK_NULL_HANDLE);
        };

        return func;
    }
}  // namespace sol
