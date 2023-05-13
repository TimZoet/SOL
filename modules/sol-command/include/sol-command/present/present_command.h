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
#include "sol-core/vulkan_swapchain.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class PresentCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PresentCommand();

        PresentCommand(const PresentCommand&) = delete;

        PresentCommand(PresentCommand&&) = delete;

        ~PresentCommand() noexcept override;

        PresentCommand& operator=(const PresentCommand&) = delete;

        PresentCommand& operator=(PresentCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanSwapchain* getSwapchain() const noexcept;

        [[nodiscard]] const std::vector<VulkanSemaphore*>& getWaitSemaphores() const noexcept;

        [[nodiscard]] const uint32_t* getWaitSemaphoreIndexPtr() const noexcept;

        [[nodiscard]] const uint32_t* getImageIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setSwapchain(VulkanSwapchain& chain);

        void addWaitSemaphore(VulkanSemaphore& semaphore);

        void setWaitSemaphoreIndexPtr(uint32_t* ptr);

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

        uint32_t* semaphoreIndexPtr = nullptr;

        uint32_t* imageIndexPtr = nullptr;

        std::function<void(VulkanSwapchain&)> recreateFunction;
    };
}  // namespace sol