#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/vulkan_swapchain.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/i_task.h"

namespace sol
{
    class PresentTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PresentTask();

        PresentTask(std::string                           taskName,
                    ITaskResource<VulkanSwapchain>*       swap,
                    ITaskResource<uint32_t>*              imageIdx,
                    ITaskResource<VulkanQueue>*           queue,
                    ITaskResourceList<VulkanSemaphore>*   wait,
                    std::function<void(VulkanSwapchain&)> recreate);

        PresentTask(const PresentTask&) = delete;

        PresentTask(PresentTask&&) noexcept;

        ~PresentTask() noexcept override;

        PresentTask& operator=(const PresentTask&) = delete;

        PresentTask& operator=(PresentTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<VulkanSwapchain>* swapchain = nullptr;

        ITaskResource<uint32_t>* imageIndex = nullptr;

        ITaskResource<VulkanQueue>* presentQueue = nullptr;

        ITaskResourceList<VulkanSemaphore>* waitSemaphores = nullptr;

        std::function<void(VulkanSwapchain&)> recreateFunction;
    };
}  // namespace sol