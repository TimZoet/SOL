#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"
#include "sol-render/graphics/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/i_command.h"

namespace sol
{
    class ForwardRenderCommand final : public ICommand
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardRenderCommand();

        ForwardRenderCommand(const ForwardRenderCommand&) = delete;

        ForwardRenderCommand(ForwardRenderCommand&&) = delete;

        ~ForwardRenderCommand() noexcept override;

        ForwardRenderCommand& operator=(const ForwardRenderCommand&) = delete;

        ForwardRenderCommand& operator=(ForwardRenderCommand&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] ForwardRenderer* getRenderer() const noexcept;

        [[nodiscard]] VulkanRenderPass* getRenderPass() const noexcept;

        [[nodiscard]] VulkanCommandBufferList* getCommandBufferList() const noexcept;

        // TODO: This is a hack because we need to recreate framebuffers when the swapchain is recreated.
        [[nodiscard]] std::vector<VulkanFramebuffer*>& getFramebuffers() noexcept;

        [[nodiscard]] const std::vector<VulkanFramebuffer*>& getFramebuffers() const noexcept;

        [[nodiscard]] const uint32_t* getFrameIndexPtr() const noexcept;

        [[nodiscard]] const uint32_t* getImageIndexPtr() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setRenderer(ForwardRenderer& render);

        void setRenderData(ForwardRenderData& data);

        void setRenderPass(VulkanRenderPass& pass);

        void setCommandBufferList(VulkanCommandBufferList& buffers);

        void addFramebuffer(VulkanFramebuffer& framebuffer);

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

        ForwardRenderer* renderer = nullptr;

        ForwardRenderData* renderData = nullptr;

        VulkanRenderPass* renderPass = nullptr;

        VulkanCommandBufferList* commandBuffers = nullptr;

        std::vector<VulkanFramebuffer*> framebuffers;

        uint32_t* frameIndexPtr = nullptr;

        uint32_t* imageIndexPtr = nullptr;
    };
}  // namespace sol