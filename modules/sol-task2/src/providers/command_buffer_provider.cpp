#include "sol-task/providers/command_buffer_provider.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_fence.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/compiled_graph.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    CommandBufferProvider::CommandBufferProvider(CompiledGraph& g, const uint32_t count) : IProvider(g), index(g, count)
    {
        // TODO: Creation of actual resoures should be delayed until graph has been fully optimized.
        {
            const VulkanCommandBuffer::Settings settings{.commandPool = g.getCommandPool()};
            commandBuffers = VulkanCommandBuffer::create(settings, count);
        }
        {
            const VulkanFence::Settings settings{.device = g.getDevice(), .signaled = true};
            fences = VulkanFence::create(settings, count);
        }
    }

    CommandBufferProvider::~CommandBufferProvider() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    uint32_t CommandBufferProvider::getIndex() const { return index.getValue(); }

    VulkanCommandBuffer& CommandBufferProvider::get() const { return *commandBuffers[getIndex()]; }

    VulkanFence* CommandBufferProvider::getFence() const { return fences[getIndex()].get(); }

    ////////////////////////////////////////////////////////////////
    // Graph setup.
    ////////////////////////////////////////////////////////////////

    void CommandBufferProvider::createResources()
    {
        {
            const VulkanCommandBuffer::Settings settings{.commandPool = getGraph().getCommandPool()};
            commandBuffers = VulkanCommandBuffer::create(settings, index.getRange());
        }

        {
            const VulkanFence::Settings settings{.device = getGraph().getDevice(), .signaled = true};
            fences = VulkanFence::create(settings, index.getRange());
        }
    }

    void CommandBufferProvider::loop() { index.increment(); }
}  // namespace sol
