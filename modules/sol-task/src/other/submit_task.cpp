#include "sol-task/other/submit_task.h"

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
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"
#include "sol-task/i_task_resource_list.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    SubmitTask::SubmitTask() = default;

    SubmitTask::SubmitTask(std::string                              taskName,
                           ITaskResource<VulkanQueue>*              queue,
                           ITaskResourceList<VulkanCommandBuffer>*  commandBuffers,
                           ITaskResourceList<VulkanSemaphore>*      waitSems,
                           ITaskResourceList<VkPipelineStageFlags>* waitFlags,
                           ITaskResourceList<VulkanSemaphore>*      signalSems,
                           ITaskResource<VulkanFence>*              fence) :
        ITask(std::move(taskName)),
        submitQueue(queue),
        commandBuffers(commandBuffers),
        waitSemaphores(waitSems),
        waitFlags(waitFlags),
        signalSemaphores(signalSems),
        signalFence(fence)
    {
    }

    SubmitTask::SubmitTask(SubmitTask&&) noexcept = default;

    SubmitTask::~SubmitTask() noexcept = default;

    SubmitTask& SubmitTask::operator=(SubmitTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void SubmitTask::finalize()
    {
        // TODO: Reinstance these checks here and in other tasks.
        /*if (!submitQueue) throw SolError("Cannot finalize SubmitTask: queue not set.");
        if (commandBuffers.empty() && (!commandBufferList || commandBufferList->getSize() == 0))
            throw SolError("Cannot finalize SubmitTask: no command buffers.");
        if (signalFences.size() > 1 && !signalFenceIndexPtr)
            throw SolError(
              "Cannot finalize SubmitTask: signalFenceIndexPtr is null while there are more than 1 fences.");*/
    }

    void SubmitTask::operator()()
    {
        std::vector<VkSemaphore> waitSemaphoreHandles;
        if (waitSemaphores)
            waitSemaphoreHandles = *waitSemaphores |
                                   std::views::transform([](const VulkanSemaphore& s) { return s.get(); }) |
                                   std::ranges::to<std::vector>();

        std::vector<VkPipelineStageFlags> waitFs;
        if (waitFlags) waitFs = *waitFlags | std::ranges::to<std::vector>();
        const auto bufferHandles = *commandBuffers |
                                   std::views::transform([](const VulkanCommandBuffer& b) { return b.get(); }) |
                                   std::ranges::to<std::vector>();

        const auto signalSemaphoreHandles = *signalSemaphores |
                                            std::views::transform([](const VulkanSemaphore& s) { return s.get(); }) |
                                            std::ranges::to<std::vector>();

        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = static_cast<uint32_t>(waitSemaphoreHandles.size());
        submitInfo.pWaitSemaphores      = waitSemaphoreHandles.data();
        submitInfo.pWaitDstStageMask    = waitFs.data();
        submitInfo.commandBufferCount   = static_cast<uint32_t>(bufferHandles.size());
        submitInfo.pCommandBuffers      = bufferHandles.data();
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphoreHandles.size());
        submitInfo.pSignalSemaphores    = signalSemaphoreHandles.data();

        handleVulkanError(vkQueueSubmit(
          (*submitQueue)->get(), 1, &submitInfo, signalFence->empty() ? VK_NULL_HANDLE : (*signalFence)->get()));
    }
}  // namespace sol
