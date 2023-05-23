#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-render/compute/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class ComputeRenderCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeRenderCommand();

        ComputeRenderCommand(const ComputeRenderCommand&) = delete;

        ComputeRenderCommand(ComputeRenderCommand&&) = delete;

        ~ComputeRenderCommand() noexcept override;

        ComputeRenderCommand& operator=(const ComputeRenderCommand&) = delete;

        ComputeRenderCommand& operator=(ComputeRenderCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] ComputeRenderer* getRenderer() const noexcept;

        [[nodiscard]] ComputeRenderData* getRenderData() const noexcept;

        [[nodiscard]] VulkanCommandBufferList* getCommandBufferList() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setRenderer(ComputeRenderer& render);

        void setRenderData(ComputeRenderData& data);

        void setCommandBufferList(VulkanCommandBufferList& buffers);

        void setFrameIndexPtr(uint32_t* ptr);

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

        ComputeRenderer* renderer = nullptr;

        ComputeRenderData* renderData = nullptr;

        VulkanCommandBufferList* commandBuffers = nullptr;

        uint32_t* frameIndexPtr = nullptr;
    };
}  // namespace sol
