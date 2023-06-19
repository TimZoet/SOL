#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-render/graphics/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class GraphicsRenderCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsRenderCommand();

        GraphicsRenderCommand(const GraphicsRenderCommand&) = delete;

        GraphicsRenderCommand(GraphicsRenderCommand&&) = delete;

        ~GraphicsRenderCommand() noexcept override;

        GraphicsRenderCommand& operator=(const GraphicsRenderCommand&) = delete;

        GraphicsRenderCommand& operator=(GraphicsRenderCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] GraphicsRenderer* getRenderer() const noexcept;

        [[nodiscard]] const std::vector<GraphicsRenderingInfo*>& getRenderingInfos() const noexcept;

        [[nodiscard]] VulkanCommandBufferList* getCommandBufferList() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        [[nodiscard]] const uint32_t* getImageIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setRenderer(GraphicsRenderer& render);

        void setRenderData(GraphicsRenderData& data);

        void addRenderingInfo(GraphicsRenderingInfo& info);

        void setCommandBufferList(VulkanCommandBufferList& buffers);

        void setFrameIndexPtr(uint32_t* ptr);

        void setImageIndexPtr(uint32_t* ptr);

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

        GraphicsRenderer* renderer = nullptr;

        GraphicsRenderData* renderData = nullptr;

        std::vector<GraphicsRenderingInfo*> renderingInfos;

        VulkanCommandBufferList* commandBuffers = nullptr;

        uint32_t* frameIndexPtr = nullptr;

        uint32_t* imageIndexPtr = nullptr;
    };
}  // namespace sol