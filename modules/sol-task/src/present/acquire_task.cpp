#include "sol-task/present/acquire_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_swapchain.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"
#include "sol-task/task_graph.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    AcquireTask::AcquireTask() = default;

    AcquireTask::AcquireTask(std::string                           taskName,
                             ITaskResource<VulkanSwapchain>*       swap,
                             ITaskResource<VulkanSemaphore>*       signalS,
                             ITaskResource<VulkanFence>*           signalF,
                             ITaskResource<uint32_t>*              imageIdx,
                             std::function<void(VulkanSwapchain&)> recreate) :
        ITask(std::move(taskName)),
        swapchain(swap),
        signalSemaphore(signalS),
        signalFence(signalF),
        imageIndex(imageIdx),
        recreateFunction(std::move(recreate))
    {
    }

    AcquireTask::AcquireTask(AcquireTask&&) noexcept = default;

    AcquireTask::~AcquireTask() noexcept = default;

    AcquireTask& AcquireTask::operator=(AcquireTask&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void AcquireTask::finalize()
    {
        /*if (!swapchain) throw SolError("Cannot finalize AcquireTask: swapchain not set.");
        if (semaphores.size() > 1 && !semaphoreIndexPtr)
            throw SolError(
              "Cannot finalize AcquireTask: semaphoreIndexPtr is null while there are more than 1 semaphores.");
        if (fences.size() > 1 && !fenceIndexPtr)
            throw SolError("Cannot finalize AcquireTask: fenceIndexPtr is null while there are more than 1 fences.");*/
    }

    void AcquireTask::operator()()
    {
        VkSemaphore signalS = VK_NULL_HANDLE;
        if (signalSemaphore && !signalSemaphore->empty()) signalS = (*signalSemaphore)->get();
        VkFence signalF = VK_NULL_HANDLE;
        if (signalFence && !signalFence->empty()) signalF = (*signalFence)->get();

        // Acquire image.
        auto&      rImageIndex = **imageIndex;
        const auto result      = vkAcquireNextImageKHR(taskGraph->getDevice().get(),
                                                  (*swapchain)->get(),
                                                  std::numeric_limits<uint64_t>::max(),
                                                  signalS,
                                                  signalF,
                                                  &rImageIndex);

        // TODO: Add member variable with function to recreate swapchain.
        if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR
        {
            vkDeviceWaitIdle(taskGraph->getDevice().get());
            (*swapchain)->recreate();
            if (recreateFunction) recreateFunction(**swapchain);
        }
        //if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) handleVulkanError(result);
    }
}  // namespace sol
