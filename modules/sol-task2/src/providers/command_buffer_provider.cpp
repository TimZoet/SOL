#include "sol-task/providers/command_buffer_provider.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_semaphore.h"

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
        {
            const VulkanCommandBuffer::Settings settings{.commandPool = g.getCommandPool()};
            commandBuffers = VulkanCommandBuffer::create(settings, count);
        }
        {
            const VulkanSemaphore::Settings settings{.device = g.getDevice()};
            semaphores = VulkanSemaphore::create(settings, count);
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

    VulkanSemaphore* CommandBufferProvider::getSemaphore() const { return semaphores[getIndex()].get(); }

    VulkanFence* CommandBufferProvider::getFence() const { return fences[getIndex()].get(); }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void CommandBufferProvider::increment() { index.increment(); }
}  // namespace sol
