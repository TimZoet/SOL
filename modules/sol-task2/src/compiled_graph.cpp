#include "sol-task/compiled_graph.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_semaphore.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/providers/command_buffer_provider.h"
#include "sol-task/providers/index_provider.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    CompiledGraph::CompiledGraph(VulkanDevice& dev, VulkanCommandPool& cPool) :
        device(&dev), commandPool(&cPool), finished(2)
    {
    }

    CompiledGraph::~CompiledGraph() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& CompiledGraph::getDevice() noexcept { return *device; }

    const VulkanDevice& CompiledGraph::getDevice() const noexcept { return *device; }

    VulkanCommandPool& CompiledGraph::getCommandPool() noexcept { return *commandPool; }

    const VulkanCommandPool& CompiledGraph::getCommandPool() const noexcept { return *commandPool; }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void CompiledGraph::start()
    {
        // Reset counters and make all nodes without execution dependencies available.
        for (auto& node : nodes)
        {
            node->wait = static_cast<uint32_t>(node->dependencies.size());

            // Make nodes without execution dependencies immediately available.
            if (node->dependencies.empty()) available.emplace_back(node.get());
        }
    }

    std::optional<std::function<void()>> CompiledGraph::getNext()
    {
        std::scoped_lock lock(mutex);

        if (available.empty()) { return {}; }

        auto* node = available.back();
        available.pop_back();

        return [node, this] {
            // Run task.
            if (node->f) node->f.value()();
            // This is the final node.
            else
            {
                //finished.arrive_and_wait();
                static_cast<void>(finished.arrive());
                return;
            }

            // Notify dependencies of completion. If counter reaches 0, make node available.
            for (auto* n : node->dependents)
            {
                if (--n->wait == 0) makeAvailable(*n);
            }
        };
    }

    void CompiledGraph::end()
    {
        // Wait on final node.
        finished.arrive_and_wait();

        for (const auto& cb : resources.commandBuffers) cb->increment();
    }

    void CompiledGraph::makeAvailable(Node& node) noexcept
    {
        std::scoped_lock lock(mutex);
        available.emplace_back(&node);
    }
}  // namespace sol
