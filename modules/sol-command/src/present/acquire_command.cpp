#include "sol-command/present/acquire_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    AcquireCommand::AcquireCommand() = default;

    AcquireCommand::~AcquireCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanSwapchain* AcquireCommand::getSwapchain() const noexcept { return swapchain; }

    const std::vector<VulkanSemaphore*>& AcquireCommand::getSignalSemaphores() const noexcept { return semaphores; }

    const uint32_t* AcquireCommand::getSignalSemaphoreIndexPtr() const noexcept { return semaphoreIndexPtr; }

    const std::vector<VulkanFence*>& AcquireCommand::getSignalFences() const noexcept { return fences; }

    const uint32_t* AcquireCommand::getSignalFenceIndexPtr() const noexcept { return fenceIndexPtr; }

    const uint32_t* AcquireCommand::getImageIndexPtr() const noexcept { return imageIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void AcquireCommand::setSwapchain(VulkanSwapchain& chain)
    {
        commandQueue->requireNonFinalized();
        swapchain = &chain;
    }

    void AcquireCommand::addSignalSemaphore(VulkanSemaphore& semaphore)
    {
        commandQueue->requireNonFinalized();
        semaphores.emplace_back(&semaphore);
    }

    void AcquireCommand::setSignalSemaphoreIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        semaphoreIndexPtr = ptr;
    }

    void AcquireCommand::addSignalFence(VulkanFence& fence)
    {
        commandQueue->requireNonFinalized();
        fences.emplace_back(&fence);
    }

    void AcquireCommand::setSignalFenceIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        fenceIndexPtr = ptr;
    }

    void AcquireCommand::setImageIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        imageIndexPtr = ptr;
    }

    void AcquireCommand::setRecreateFunction(std::function<void(VulkanSwapchain&)> f)
    {
        recreateFunction = std::move(f);
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void AcquireCommand::finalize()
    {
        if (!swapchain) throw SolError("Cannot finalize AcquireCommand: swapchain not set.");
        if (semaphores.size() > 1 && !semaphoreIndexPtr)
            throw SolError(
              "Cannot finalize AcquireCommand: semaphoreIndexPtr is null while there are more than 1 semaphores.");
        if (fences.size() > 1 && !fenceIndexPtr)
            throw SolError("Cannot finalize AcquireCommand: fenceIndexPtr is null while there are more than 1 fences.");
    }

    void AcquireCommand::operator()()
    {
        // Try to get semaphore.
        VkSemaphore semaphore = VK_NULL_HANDLE;
        if (!semaphores.empty())
        {
            if (semaphoreIndexPtr)
            {
                if (*semaphoreIndexPtr >= semaphores.size())
                    throw SolError("Cannot run AcquireCommand: semaphoreIndexPtr is out of bounds.");

                semaphore = semaphores[*semaphoreIndexPtr]->get();
            }
            else
                semaphore = semaphores.front()->get();
        }

        // Try to get fence.
        VkFence fence = VK_NULL_HANDLE;
        if (!fences.empty())
        {
            if (fenceIndexPtr)
            {
                if (*fenceIndexPtr >= fences.size())
                    throw SolError("Cannot run AcquireCommand: fenceIndexPtr is out of bounds.");

                fence = fences[*fenceIndexPtr]->get();
            }
            else
                fence = fences.front()->get();
        }

        // Acquire image.
        const auto result = vkAcquireNextImageKHR(swapchain->getDevice().get(),
                                                  swapchain->get(),
                                                  std::numeric_limits<uint64_t>::max(),
                                                  semaphore,
                                                  fence,
                                                  imageIndexPtr);

        // TODO: Add member variable with function to recreate swapchain.
        if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR
        {
            vkDeviceWaitIdle(swapchain->getDevice().get());
            swapchain->recreate();
            if (recreateFunction) recreateFunction(*swapchain);
        }
        //if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) handleVulkanError(result);
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string AcquireCommand::getVizLabel() const { return "Acquire"; }
}  // namespace sol
