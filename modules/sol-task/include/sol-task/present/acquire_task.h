#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>

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
    class AcquireTask final : public ITask
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        AcquireTask();

        AcquireTask(std::string                           taskName,
                    ITaskResource<VulkanSwapchain>*       swap,
                    ITaskResource<VulkanSemaphore>*       signalS,
                    ITaskResource<VulkanFence>*           signalF,
                    ITaskResource<uint32_t>*              imageIdx,
                    std::function<void(VulkanSwapchain&)> recreate);

        AcquireTask(const AcquireTask&) = delete;

        AcquireTask(AcquireTask&&) noexcept;

        ~AcquireTask() noexcept override;

        AcquireTask& operator=(const AcquireTask&) = delete;

        AcquireTask& operator=(AcquireTask&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ITaskResource<VulkanSwapchain>* swapchain = nullptr;

        ITaskResource<VulkanSemaphore>* signalSemaphore = nullptr;

        ITaskResource<VulkanFence>* signalFence = nullptr;

        ITaskResource<uint32_t>* imageIndex = nullptr;

        std::function<void(VulkanSwapchain&)> recreateFunction;
    };
}  // namespace sol
