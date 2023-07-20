#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/i_task.h"

namespace sol
{
    class SubmitTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SubmitTask();

        SubmitTask(std::string                              taskName,
                   ITaskResource<VulkanQueue>*              queue,
                   ITaskResourceList<VulkanCommandBuffer>*  commandBuffers,
                   ITaskResourceList<VulkanSemaphore>*      waitSems,
                   ITaskResourceList<VkPipelineStageFlags>* waitFlags,
                   ITaskResourceList<VulkanSemaphore>*      signalSems,
                   ITaskResource<VulkanFence>*              fence);

        SubmitTask(const SubmitTask&) = delete;

        SubmitTask(SubmitTask&&) noexcept;

        ~SubmitTask() noexcept override;

        SubmitTask& operator=(const SubmitTask&) = delete;

        SubmitTask& operator=(SubmitTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<VulkanQueue>* submitQueue = nullptr;

        ITaskResourceList<VulkanCommandBuffer>* commandBuffers = nullptr;

        ITaskResourceList<VulkanSemaphore>* waitSemaphores = nullptr;

        ITaskResourceList<VkPipelineStageFlags>* waitFlags = nullptr;

        ITaskResourceList<VulkanSemaphore>* signalSemaphores = nullptr;

        ITaskResource<VulkanFence>* signalFence = nullptr;
    };
}  // namespace sol
