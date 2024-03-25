#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/providers/i_provider.h"
#include "sol-task/providers/index_provider.h"

namespace sol
{
    class CommandBufferProvider final : public IProvider
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        CommandBufferProvider() = delete;

        CommandBufferProvider(CompiledGraph& g, uint32_t count);

        CommandBufferProvider(const CommandBufferProvider&) = delete;

        CommandBufferProvider(CommandBufferProvider&&) = delete;

        ~CommandBufferProvider() noexcept override;

        CommandBufferProvider& operator=(const CommandBufferProvider&) = delete;

        CommandBufferProvider& operator=(CommandBufferProvider&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] uint32_t getIndex() const;

        [[nodiscard]] VulkanCommandBuffer& get() const;

        [[nodiscard]] VulkanSemaphore* getSemaphore() const;

        [[nodiscard]] VulkanFence* getFence() const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void increment();

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IndexProvider index;

        std::vector<VulkanCommandBufferPtr> commandBuffers;

        std::vector<VulkanSemaphorePtr> semaphores;

        std::vector<VulkanFencePtr> fences;
    };
}  // namespace sol
