#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class AcquireCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        AcquireCommand();

        AcquireCommand(const AcquireCommand&) = delete;

        AcquireCommand(AcquireCommand&&) = delete;

        ~AcquireCommand() noexcept override;

        AcquireCommand& operator=(const AcquireCommand&) = delete;

        AcquireCommand& operator=(AcquireCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanSwapchain* getSwapchain() const noexcept;

        [[nodiscard]] const std::vector<VulkanSemaphore*>& getSignalSemaphores() const noexcept;

        [[nodiscard]] const uint32_t* getSignalSemaphoreIndexPtr() const noexcept;

        [[nodiscard]] const std::vector<VulkanFence*>& getSignalFences() const noexcept;

        [[nodiscard]] const uint32_t* getSignalFenceIndexPtr() const noexcept;

        [[nodiscard]] const uint32_t* getImageIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setSwapchain(VulkanSwapchain& chain);

        void addSignalSemaphore(VulkanSemaphore& semaphore);

        void setSignalSemaphoreIndexPtr(uint32_t* ptr);

        void addSignalFence(VulkanFence& fence);

        void setSignalFenceIndexPtr(uint32_t* ptr);

        void setImageIndexPtr(uint32_t* ptr);

        void setRecreateFunction(std::function<void(VulkanSwapchain&)> f);

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        void finalize() override;

        void operator()() override;

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string getVizLabel() const override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanSwapchain* swapchain = nullptr;

        std::vector<VulkanSemaphore*> semaphores;

        std::vector<VulkanFence*> fences;

        uint32_t* semaphoreIndexPtr = nullptr;

        uint32_t* fenceIndexPtr = nullptr;

        uint32_t* imageIndexPtr = nullptr;

        std::function<void(VulkanSwapchain&)> recreateFunction;
    };
}  // namespace sol