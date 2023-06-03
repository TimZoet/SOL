#include "sol-command/present/present_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    PresentCommand::PresentCommand() = default;

    PresentCommand::~PresentCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanSwapchain* PresentCommand::getSwapchain() const noexcept { return swapchain; }

    const std::vector<VulkanSemaphore*>& PresentCommand::getWaitSemaphores() const noexcept { return semaphores; }

    const uint32_t* PresentCommand::getWaitSemaphoreIndexPtr() const noexcept { return semaphoreIndexPtr; }

    const uint32_t* PresentCommand::getImageIndexPtr() const noexcept { return imageIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void PresentCommand::setSwapchain(VulkanSwapchain& chain)
    {
        commandQueue->requireNonFinalized();
        swapchain = &chain;
    }

    void PresentCommand::addWaitSemaphore(VulkanSemaphore& semaphore)
    {
        commandQueue->requireNonFinalized();
        semaphores.emplace_back(&semaphore);
    }

    void PresentCommand::setWaitSemaphoreIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        semaphoreIndexPtr = ptr;
    }

    void PresentCommand::setImageIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        imageIndexPtr = ptr;
    }

    void PresentCommand::setRecreateFunction(std::function<void(VulkanSwapchain&)> f)
    {
        recreateFunction = std::move(f);
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void PresentCommand::finalize()
    {
        if (!swapchain) throw SolError("Cannot finalize PresentCommand: swapchain not set.");
        if (!imageIndexPtr) throw SolError("Cannot finalize PresentCommand: imageIndexPtr not set.");
    }

    void PresentCommand::operator()()
    {
        // Get all semaphore handles.
        std::vector<VkSemaphore> semaphoreHandles;
        if (!semaphores.empty())
        {
            if (semaphoreIndexPtr)
            {
                if (*semaphoreIndexPtr >= semaphores.size())
                    throw SolError("Cannot run PresentCommand: semaphoreIndexPtr is out of bounds.");

                semaphoreHandles.emplace_back(semaphores[*semaphoreIndexPtr]->get());
            }
            else
            {
                semaphoreHandles.reserve(semaphores.size());
                std::ranges::transform(
                  semaphores, std::back_inserter(semaphoreHandles), [&](const auto& f) { return f->get(); });
            }
        }

        VkPresentInfoKHR     presentInfo{};
        const VkSwapchainKHR swapchains[] = {swapchain->get()};
        presentInfo.sType                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount    = static_cast<uint32_t>(semaphoreHandles.size());
        presentInfo.pWaitSemaphores       = semaphoreHandles.data();
        presentInfo.swapchainCount        = 1;
        presentInfo.pSwapchains           = swapchains;
        presentInfo.pImageIndices         = imageIndexPtr;

        // TODO: WHere to get the present queue from, exactly?
        VkQueue presentQueue = VK_NULL_HANDLE;
        for (const auto& queue : swapchain->getDevice().getQueues())
        {
            if (queue->getFamily().supportsPresent())
            {
                presentQueue = queue->get();
                break;
            }
        }
        if (!presentQueue) throw SolError("Cannot run PresentCommand: no present queue found on device.");

        // TODO: Handle swapchain changes.
        const auto result = vkQueuePresentKHR(presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR || framebufferResized
        {
            vkDeviceWaitIdle(swapchain->getDevice().get());
            swapchain->recreate();
            if (recreateFunction) recreateFunction(*swapchain);
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { handleVulkanError(result); }
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string PresentCommand::getVizLabel() const { return "Present"; }
}  // namespace sol
