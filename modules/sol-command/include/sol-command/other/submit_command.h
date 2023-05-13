#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <vector>

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

#include "sol-command/i_command.h"

namespace sol
{
    class SubmitCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SubmitCommand();

        SubmitCommand(const SubmitCommand&) = delete;

        SubmitCommand(SubmitCommand&&) = delete;

        ~SubmitCommand() noexcept override;

        SubmitCommand& operator=(const SubmitCommand&) = delete;

        SubmitCommand& operator=(SubmitCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanQueue* getQueue() const noexcept;

        [[nodiscard]] const std::vector<VulkanCommandBuffer*>& getCommandBuffers() const noexcept;

        [[nodiscard]] VulkanCommandBufferList* getCommandBufferList() const noexcept;

        [[nodiscard]] const uint32_t* getCommandBufferIndexPtr() const noexcept;

        [[nodiscard]] const std::vector<VulkanSemaphore*>& getWaitSemaphores() const noexcept;

        //[[nodiscard]] const uint32_t* getWaitSemaphoreIndexPtr() const noexcept;

        [[nodiscard]] const std::vector<VulkanSemaphore*>& getSignalSemaphores() const noexcept;

        [[nodiscard]] const uint32_t* getSignalSemaphoreIndexPtr() const noexcept;

        [[nodiscard]] const std::vector<VulkanFence*>& getSignalFences() const noexcept;

        [[nodiscard]] const uint32_t* getSignalFenceIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setQueue(VulkanQueue& queue);

        void addCommandBuffer(VulkanCommandBuffer& buffer);

        void setCommandBufferList(VulkanCommandBufferList& buffers);

        void setCommandBufferIndexPtr(uint32_t* ptr);

        void addWaitSemaphore(VulkanSemaphore& semaphore, VkPipelineStageFlags flags);

        //void setWaitSemaphoreIndexPtr(uint32_t* ptr);
        void setWaitSemaphoreFunction(std::function<bool(uint32_t)> func);

        void addSignalSemaphore(VulkanSemaphore& semaphore);

        void setSignalSemaphoreIndexPtr(uint32_t* ptr);

        void addSignalFence(VulkanFence& fence);

        void setSignalFenceIndexPtr(uint32_t* ptr);

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

        VulkanQueue* submitQueue = nullptr;

        std::vector<VulkanCommandBuffer*> commandBuffers;

        VulkanCommandBufferList* commandBufferList = nullptr;

        std::vector<VulkanSemaphore*> waitSemaphores;

        std::vector<VkPipelineStageFlags> waitFlags;

        std::vector<VulkanSemaphore*> signalSemaphores;

        std::vector<VulkanFence*> signalFences;

        uint32_t* commandBufferIndexPtr = nullptr;

        //uint32_t* waitSemaphoreIndexPtr = nullptr;
        std::function<bool(uint32_t)> waitSemaphoreFunc;

        uint32_t* signalSemaphoreIndexPtr = nullptr;

        uint32_t* signalFenceIndexPtr = nullptr;
    };
}  // namespace sol