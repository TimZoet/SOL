#include "sol-task/task_graph.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <functional>
#include <ranges>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/compiled_graph.h"
#include "sol-task/providers/command_buffer_provider.h"
#include "sol-task/providers/index_provider.h"
#include "sol-task/resources/command_buffer_resource.h"
#include "sol-task/resources/index_resource.h"
#include "sol-task/tasks/i_task.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TaskGraph::TaskGraph(VulkanDevice& dev, VulkanCommandPool& cPool) : device(&dev), commandPool(&cPool) {}

    TaskGraph::~TaskGraph() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& TaskGraph::getDevice() noexcept { return *device; }

    const VulkanDevice& TaskGraph::getDevice() const noexcept { return *device; }

    ////////////////////////////////////////////////////////////////
    // Graph setup.
    ////////////////////////////////////////////////////////////////

    CommandBufferResource& TaskGraph::createCommandBuffer()
    {
        return *resources.commandBuffers.emplace_back(std::make_unique<CommandBufferResource>(*this));
    }

    IndexResource& TaskGraph::createIndex(const uint32_t range)
    {
        if (range == 0) throw SolError("Cannot create IndexResource with a range of 0.");
        return *resources.indices.emplace_back(std::make_unique<IndexResource>(*this, range));
    }

    CompiledGraphPtr TaskGraph::compile() const
    {
        // TODO: Check for cycles.

        if (tasks.empty()) throw SolError("Cannot compile empty TaskGraph.");

        std::unordered_map<const ITask*, CompiledGraph::Node*> nodeLookup;
        std::unordered_map<const ITaskResource*, IProvider*>   providerLookup;

        auto graph = std::make_unique<CompiledGraph>(*device, *commandPool);

        // Create node for each task.
        for (const auto& task : tasks)
        {
            auto& node = *graph->nodes.emplace_back(std::make_unique<CompiledGraph::Node>());
            node.name  = task->getName();
            nodeLookup.emplace(task.get(), &node);
        }

        // Create explicit execution dependencies and semaphores.
        for (const auto& task : tasks)
        {
            auto& dst = *nodeLookup.at(task.get());

            for (const auto* dep : task->getDependencies())
            {
                auto& src = *nodeLookup.at(dep);
                src.dependents.push_back(&dst);
                dst.dependencies.push_back(&src);
            }

            for (const auto* dep : task->getAwaits())
            {
                auto&                     src = *nodeLookup.at(dep);
                VulkanSemaphore::Settings settings{.device = device};
                auto& sem  = *graph->resources.semaphores.emplace_back(VulkanSemaphore::create(settings));
                src.signal = &sem;
                dst.await  = &sem;
            }
        }

        // Create execution dependencies for write/reads to CPU resources.
        for (auto& index : resources.indices)
        {
            const auto* writer = index->getWriter();
            if (!writer) continue;

            for (const auto* reader : index->getReaders())
            {
                auto& src = *nodeLookup.at(writer);
                auto& dst = *nodeLookup.at(reader);
                src.dependents.push_back(&dst);
                dst.dependencies.push_back(&src);
            }
        }

        // Create index providers.
        for (auto& i : resources.indices)
        {
            auto& provider =
              *graph->resources.indices.emplace_back(std::make_unique<IndexProvider>(*graph, i->getRange()));
            providerLookup.emplace(i.get(), &provider);
        }

        // Create command buffers and synchronization around them.
        for (auto& cb : resources.commandBuffers)
        {
            if (!cb->getWriter()) throw SolError("No task recording to command buffer.");

            auto& recordNode = *nodeLookup.at(cb->getWriter());
            auto& submitNode = *nodeLookup.at(&cb->getSubmitter());

            auto& provider = *graph->resources.commandBuffers.emplace_back(
              std::make_unique<CommandBufferProvider>(*graph, cb->getCount()));
            providerLookup.emplace(cb.get(), &provider);

            // Insert node to wait on fence before record.
            auto& fenceNode = *graph->nodes.emplace_back(std::make_unique<CompiledGraph::Node>());
            fenceNode.name  = "fence";
            fenceNode.dependents.push_back(&recordNode);
            recordNode.dependencies.push_back(&fenceNode);
            fenceNode.f = [&provider, device = this->device] {
                const auto* fence = provider.getFence();
                vkWaitForFences(device->get(), 1, &fence->get(), VK_TRUE, UINT64_MAX);
                vkResetFences(device->get(), 1, &fence->get());
            };

            // Waiters must wait on semaphore signalled by submitter.
            for (const auto* w : cb->getWaiters())
            {
                auto& waitNode   = *nodeLookup.at(w);
                waitNode.tmpwait = &provider;
                submitNode.dependents.push_back(&waitNode);
                waitNode.dependencies.push_back(&submitNode);
            }
        }

        // Compile tasks.
        for (const auto& task : tasks)
        {
            auto& node = *nodeLookup.at(task.get());
            node.f     = task->compile(node, providerLookup);
        }

        // Make nodes without dependents notify final node.
        auto finalNode  = std::make_unique<CompiledGraph::Node>();
        finalNode->name = "end";
        for (auto& node : graph->nodes)
        {
            if (node->dependents.empty())
            {
                node->dependents.push_back(finalNode.get());
                finalNode->dependencies.push_back(node.get());
            }
        }
        graph->nodes.emplace_back(std::move(finalNode));

        return graph;
    }
}  // namespace sol
