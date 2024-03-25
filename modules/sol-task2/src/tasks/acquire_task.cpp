#include "sol-task/tasks/acquire_task.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/task_graph.h"
#include "sol-task/providers/index_provider.h"
#include "sol-task/resources/index_resource.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    AcquireTask::AcquireTask(TaskGraph& taskGraph) : ITask(taskGraph) {}

    AcquireTask::~AcquireTask() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool AcquireTask::supportsCapability(const Capability capability) const noexcept
    {
        switch (capability)
        {
        case Capability::SignalSemaphore: return true;
        default: return false;
        }
    }

    IndexResource& AcquireTask::getImageIndex() const
    {
        if (!index) throw SolError("Cannot get index. It was not created yet.");
        return *index;
    }

    void AcquireTask::setQueue(VulkanQueue& q) { queue = &q; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void AcquireTask::setSwapchain(VulkanSwapchain& chain)
    {
        if (swapchain) throw SolError("Cannot set swapchain. It was already set.");
        swapchain = &chain;
        index     = &getTaskGraph().createIndex(static_cast<uint32_t>(swapchain->getImageCount()));
        index->setWriter(*this);
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    std::function<void()> AcquireTask::compile(const CompiledGraph::Node&                            node,
                                               std::unordered_map<const ITaskResource*, IProvider*>& providerLookup)
    {
        auto func = [&node,
                     indexProvider = static_cast<IndexProvider*>(providerLookup[index]),
                     queue         = this->queue,
                     swapchain     = this->swapchain] {
            uint32_t          imageIndex = 0;
            const VkSemaphore semaphore  = node.signal->get();
            const auto        result     = vkAcquireNextImageKHR(swapchain->getDevice().get(),
                                                      swapchain->get(),
                                                      std::numeric_limits<uint64_t>::max(),
                                                      semaphore,
                                                      VK_NULL_HANDLE,
                                                      &imageIndex);

            indexProvider->set(imageIndex);

            // TODO: Add member variable with function to recreate swapchain.
            if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR
            {
                /*vkDeviceWaitIdle(taskGraph->getDevice().get());
                (*swapchain)->recreate();
                if (recreateFunction) recreateFunction(**swapchain);*/
            }
            //if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) handleVulkanError(result);
        };

        return func;
    }
}  // namespace sol
