#include "sol-task/tasks/present_task.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/providers/command_buffer_provider.h"
#include "sol-task/providers/index_provider.h"
#include "sol-task/resources/index_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    PresentTask::PresentTask(TaskGraph& taskGraph) : ITask(taskGraph) {}

    PresentTask::~PresentTask() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool PresentTask::supportsCapability(const Capability capability) const noexcept
    {
        return capability == Capability::AwaitSemaphore;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void PresentTask::setQueue(VulkanQueue& q) { queue = &q; }

    void PresentTask::setSwapchain(VulkanSwapchain& chain, IndexResource& i)
    {
        swapchain = &chain;
        index     = &i;
        addRead(*index);
    }

    ////////////////////////////////////////////////////////////////
    // Compile.
    ////////////////////////////////////////////////////////////////

    std::function<void()> PresentTask::compile(const CompiledGraph::Node&                            node,
                                               std::unordered_map<const ITaskResource*, IProvider*>& providerLookup)
    {
        auto func = [&node,
                     indexProvider = static_cast<IndexProvider*>(providerLookup[index]),
                     queue         = this->queue,
                     swapchain     = this->swapchain] {
            std::vector<VkSemaphore> waitSemaphores;
            waitSemaphores.push_back(node.tmpwait->getSemaphore()->get());

            const uint32_t imageIndex = indexProvider->getValue();

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
            presentInfo.pWaitSemaphores    = waitSemaphores.data();
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &swapchain->get();
            presentInfo.pImageIndices      = &imageIndex;

            // TODO: Handle swapchain changes.
            const auto result = vkQueuePresentKHR(queue->get(), &presentInfo);
            if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR || framebufferResized
            {
                /*vkDeviceWaitIdle(getDevice().get());
                rSwapchain.recreate();
                if (recreateFunction) recreateFunction(rSwapchain);*/
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { handleVulkanError(result); }
        };

        return func;
    }
}  // namespace sol
