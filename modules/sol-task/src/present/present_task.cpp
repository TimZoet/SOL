#include "sol-task/present/present_task.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_swapchain.h"
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

    PresentTask::PresentTask() = default;

    PresentTask::PresentTask(std::string                           taskName,
                             ITaskResource<VulkanSwapchain>*       swap,
                             ITaskResource<uint32_t>*              imageIdx,
                             ITaskResource<VulkanQueue>*           queue,
                             ITaskResourceList<VulkanSemaphore>*   wait,
                             std::function<void(VulkanSwapchain&)> recreate) :
        ITask(std::move(taskName)),
        swapchain(swap),
        imageIndex(imageIdx),
        presentQueue(queue),
        waitSemaphores(wait),
        recreateFunction(std::move(recreate))
    {
    }

    PresentTask::PresentTask(PresentTask&&) noexcept = default;

    PresentTask::~PresentTask() noexcept = default;

    PresentTask& PresentTask::operator=(PresentTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void PresentTask::finalize()
    {
        /*if (!swapchain) throw SolError("Cannot finalize PresentTask: swapchain not set.");
        if (!imageIndexPtr) throw SolError("Cannot finalize PresentTask: imageIndexPtr not set.");*/
    }

    void PresentTask::operator()()
    {
        std::vector<VkSemaphore> waitSemaphoreHandles;
        if (waitSemaphores)
            waitSemaphoreHandles = *waitSemaphores |
                                   std::views::transform([](const VulkanSemaphore& s) { return s.get(); }) |
                                   std::ranges::to<std::vector>();

        const auto  rImageIndex = **imageIndex;
        auto&       rSwapchain  = **swapchain;
        const auto& rQueue      = **presentQueue;

        VkPresentInfoKHR     presentInfo{};
        const VkSwapchainKHR swapchains[] = {rSwapchain.get()};
        presentInfo.sType                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount    = static_cast<uint32_t>(waitSemaphoreHandles.size());
        presentInfo.pWaitSemaphores       = waitSemaphoreHandles.data();
        presentInfo.swapchainCount        = 1;
        presentInfo.pSwapchains           = swapchains;
        presentInfo.pImageIndices         = &rImageIndex;

        // TODO: Handle swapchain changes.
        const auto result = vkQueuePresentKHR(rQueue.get(), &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR || framebufferResized
        {
            vkDeviceWaitIdle(getDevice().get());
            rSwapchain.recreate();
            if (recreateFunction) recreateFunction(rSwapchain);
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { handleVulkanError(result); }
    }
}  // namespace sol
