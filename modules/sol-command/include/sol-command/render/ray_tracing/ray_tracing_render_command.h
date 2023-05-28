#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-render/ray_tracing/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class RayTracingRenderCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingRenderCommand();

        RayTracingRenderCommand(const RayTracingRenderCommand&) = delete;

        RayTracingRenderCommand(RayTracingRenderCommand&&) = delete;

        ~RayTracingRenderCommand() noexcept override;

        RayTracingRenderCommand& operator=(const RayTracingRenderCommand&) = delete;

        RayTracingRenderCommand& operator=(RayTracingRenderCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] RayTracingRenderer* getRenderer() const noexcept;

        [[nodiscard]] RayTracingRenderData* getRenderData() const noexcept;

        [[nodiscard]] VulkanCommandBufferList* getCommandBufferList() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setRenderer(RayTracingRenderer& render);

        void setRenderData(RayTracingRenderData& data);

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

        RayTracingRenderer* renderer = nullptr;

        RayTracingRenderData* renderData = nullptr;

        VulkanCommandBufferList* commandBuffers = nullptr;

        uint32_t* frameIndexPtr = nullptr;
    };
}  // namespace sol
