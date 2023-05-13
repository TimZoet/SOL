#include "sol-command/other/submit_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_semaphore.h"
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

    SubmitCommand::SubmitCommand()
    {
        waitSemaphoreFunc = [](uint32_t) { return true; };
    }

    SubmitCommand::~SubmitCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanQueue* SubmitCommand::getQueue() const noexcept { return submitQueue; }

    const std::vector<VulkanCommandBuffer*>& SubmitCommand::getCommandBuffers() const noexcept
    {
        return commandBuffers;
    }

    VulkanCommandBufferList* SubmitCommand::getCommandBufferList() const noexcept { return commandBufferList; }

    const uint32_t* SubmitCommand::getCommandBufferIndexPtr() const noexcept { return commandBufferIndexPtr; }

    const std::vector<VulkanSemaphore*>& SubmitCommand::getWaitSemaphores() const noexcept { return waitSemaphores; }

    //const uint32_t* SubmitCommand::getWaitSemaphoreIndexPtr() const noexcept { return waitSemaphoreIndexPtr; }

    const std::vector<VulkanSemaphore*>& SubmitCommand::getSignalSemaphores() const noexcept
    {
        return signalSemaphores;
    }

    const uint32_t* SubmitCommand::getSignalSemaphoreIndexPtr() const noexcept { return signalSemaphoreIndexPtr; }

    const std::vector<VulkanFence*>& SubmitCommand::getSignalFences() const noexcept { return signalFences; }

    const uint32_t* SubmitCommand::getSignalFenceIndexPtr() const noexcept { return signalFenceIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void SubmitCommand::setQueue(VulkanQueue& queue) { submitQueue = &queue; }

    void SubmitCommand::addCommandBuffer(VulkanCommandBuffer& buffer) { commandBuffers.emplace_back(&buffer); }

    void SubmitCommand::setCommandBufferList(VulkanCommandBufferList& buffers) { commandBufferList = &buffers; }

    void SubmitCommand::setCommandBufferIndexPtr(uint32_t* ptr) { commandBufferIndexPtr = ptr; }

    void SubmitCommand::addWaitSemaphore(VulkanSemaphore& semaphore, VkPipelineStageFlags flags)
    {
        commandQueue->requireNonFinalized();
        waitSemaphores.emplace_back(&semaphore);
        waitFlags.emplace_back(flags);
    }

    /*void SubmitCommand::setWaitSemaphoreIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        waitSemaphoreIndexPtr = ptr;
    }*/

    void SubmitCommand::setWaitSemaphoreFunction(std::function<bool(uint32_t)> func)
    {
        commandQueue->requireNonFinalized();
        waitSemaphoreFunc = std::move(func);
    }

    void SubmitCommand::addSignalSemaphore(VulkanSemaphore& semaphore)
    {
        commandQueue->requireNonFinalized();
        signalSemaphores.emplace_back(&semaphore);
    }

    void SubmitCommand::setSignalSemaphoreIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        signalSemaphoreIndexPtr = ptr;
    }

    void SubmitCommand::addSignalFence(VulkanFence& fence)
    {
        commandQueue->requireNonFinalized();
        signalFences.emplace_back(&fence);
    }

    void SubmitCommand::setSignalFenceIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        signalFenceIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void SubmitCommand::finalize()
    {
        if (!submitQueue) throw SolError("Cannot finalize SubmitCommand: queue not set.");
        if (commandBuffers.empty() && (!commandBufferList || commandBufferList->getSize() == 0))
            throw SolError("Cannot finalize SubmitCommand: no command buffers.");
        if (signalFences.size() > 1 && !signalFenceIndexPtr)
            throw SolError(
              "Cannot finalize SubmitCommand: signalFenceIndexPtr is null while there are more than 1 fences.");
    }

    void SubmitCommand::operator()()
    {
        std::vector<VkCommandBuffer>      buffers;
        std::vector<VkSemaphore>          waits;
        std::vector<VkPipelineStageFlags> flags;
        std::vector<VkSemaphore>          signals;
        VkFence                           fence = VK_NULL_HANDLE;

        if (commandBufferIndexPtr)
        {
            if (commandBufferList)
            {
                if (*commandBufferIndexPtr >= commandBufferList->getSize())
                    throw SolError("Cannot run SubmitCommand: commandBufferIndexPtr is out of bounds.");
                buffers.emplace_back(commandBufferList->get(*commandBufferIndexPtr));
            }
            else
            {
                if (*commandBufferIndexPtr >= commandBuffers.size())
                    throw SolError("Cannot run SubmitCommand: commandBufferIndexPtr is out of bounds.");
                buffers.emplace_back(commandBuffers[*commandBufferIndexPtr]->get());
            }
        }
        else
        {
            if (commandBufferList)
            {
                for (size_t i = 0; i < commandBufferList->getSize(); i++)
                    buffers.emplace_back(commandBufferList->get(i));
            }
            else
                std::ranges::transform(
                  commandBuffers, std::back_inserter(buffers), [](const auto& s) { return s->get(); });
        }

        for (uint32_t i = 0; i < waitSemaphores.size(); i++)
        {
            if (waitSemaphoreFunc(i))
            {
                waits.emplace_back(waitSemaphores[i]->get());
                flags.emplace_back(waitFlags[i]);
            }
        }
        /*if (waitSemaphoreIndexPtr)
        {
            if (*waitSemaphoreIndexPtr >= waitSemaphores.size())
                throw SolError("Cannot run SubmitCommand: waitSemaphoreIndexPtr is out of bounds.");

            waits.emplace_back(waitSemaphores[*waitSemaphoreIndexPtr]->get());
            flags.emplace_back(waitFlags[*waitSemaphoreIndexPtr]);
        }
        else
        {
            std::ranges::transform(waitSemaphores, std::back_inserter(waits), [](const auto& s) { return s->get(); });
            flags = waitFlags;
        }*/

        if (signalSemaphoreIndexPtr)
        {
            if (*signalSemaphoreIndexPtr >= signalSemaphores.size())
                throw SolError("Cannot run SubmitCommand: signalSemaphoreIndexPtr is out of bounds.");

            signals.emplace_back(signalSemaphores[*signalSemaphoreIndexPtr]->get());
        }
        else
            std::ranges::transform(
              signalSemaphores, std::back_inserter(signals), [](const auto& s) { return s->get(); });

        if (signalFenceIndexPtr)
        {
            if (*signalFenceIndexPtr >= signalFences.size())
                throw SolError("Cannot run SubmitCommand: signalFenceIndexPtr is out of bounds.");

            fence = signalFences[*signalFenceIndexPtr]->get();
        }
        else if (!signalFences.empty())
            fence = signalFences.front()->get();

        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = static_cast<uint32_t>(waits.size());
        submitInfo.pWaitSemaphores      = waits.data();
        submitInfo.pWaitDstStageMask    = flags.data();
        submitInfo.commandBufferCount   = static_cast<uint32_t>(buffers.size());
        submitInfo.pCommandBuffers      = buffers.data();
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signals.size());
        submitInfo.pSignalSemaphores    = signals.data();

        handleVulkanError(vkQueueSubmit(submitQueue->get(), 1, &submitInfo, fence));
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string SubmitCommand::getVizLabel() const { return "Submit"; }
}  // namespace sol
