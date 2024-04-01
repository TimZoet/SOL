#include "sol-task/task_graph.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/compiled_graph.h"
#include "sol-task/providers/command_buffer_provider.h"
#include "sol-task/providers/index_provider.h"
#include "sol-task/providers/semaphore_provider.h"
#include "sol-task/providers/timeline_semaphore_provider.h"
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
        auto& res = *resources.commandBuffers.emplace_back(std::make_unique<CommandBufferResource>(*this));
        resources.all.push_back(&res);
        return res;
    }

    IndexResource& TaskGraph::createIndex(const uint32_t range)
    {
        if (range == 0) throw SolError("Cannot create IndexResource with a range of 0.");
        auto& res = *resources.indices.emplace_back(std::make_unique<IndexResource>(*this, range));
        resources.all.push_back(&res);
        return res;
    }

    CompiledGraphPtr TaskGraph::compile() const
    {
        if (tasks.empty()) throw SolError("Cannot compile empty TaskGraph.");

        // TODO: Check for cycles.

        // TODO: Validate tasks.
        /*for (const auto& task : tasks)
        {
            task->validate();
        }*/

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
                src.dependents.insert(&dst);
                dst.dependencies.insert(&src);
            }

            for (const auto& [dep, stages] : task->getAwaits())
            {
                auto& src = *nodeLookup.at(dep);
                auto& sem = *graph->resources.semaphores.emplace_back(std::make_unique<SemaphoreProvider>(*graph, 1));
                src.signalSemaphores.push_back(&sem);
                dst.awaitSemaphores.emplace_back(&sem, stages);
                // TODO: If signal can be done using counting semaphore, execution dependency is no longer needed.
                src.dependents.insert(&dst);
                dst.dependencies.insert(&src);
            }
        }

        // Create execution dependencies for write/reads to CPU resources.
        for (auto& res : resources.all)
        {
            const auto* writer = res->getWriter();
            if (!writer) continue;
            for (const auto* reader : res->getReaders())
            {
                auto& src = *nodeLookup.at(writer);
                auto& dst = *nodeLookup.at(reader);
                src.dependents.insert(&dst);
                dst.dependencies.insert(&src);
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

            // TODO: Could be replaced by timeline semaphore as well?
            // Insert node to wait on fence before record.
            auto& fenceNode = *graph->nodes.emplace_back(std::make_unique<CompiledGraph::Node>());
            fenceNode.name  = "fence";
            fenceNode.dependents.insert(&recordNode);
            recordNode.dependencies.insert(&fenceNode);
            fenceNode.f = [&provider, device = this->device] {
                const auto* fence = provider.getFence();
                vkWaitForFences(device->get(), 1, &fence->get(), VK_TRUE, UINT64_MAX);
                vkResetFences(device->get(), 1, &fence->get());
            };

            // Waiters must wait on semaphore signaled by submitter.
            for (const auto& [w, stages] : cb->getWaiters())
            {
                auto& waitNode = *nodeLookup.at(w);

                if (w->supportsCapability(ITask::Capability::TimelineSemaphore))
                {
                    // TODO: Tasks don't properly wait yet, so throw for now.
                    throw std::runtime_error("");
                    /*auto& sem = *graph->resources.timelineSemaphores.emplace_back(
                      std::make_unique<TimelineSemaphoreProvider>(*graph, cb->getCount()));
                    submitNode.signalTimelineSemaphores.emplace_back(&sem);
                    waitNode.awaitTimelineSemaphores.emplace_back(&sem, stages);*/
                }
                else
                {
                    auto& sem = *graph->resources.semaphores.emplace_back(
                      std::make_unique<SemaphoreProvider>(*graph, cb->getCount()));
                    submitNode.signalSemaphores.emplace_back(&sem);
                    waitNode.awaitSemaphores.emplace_back(&sem, stages);

                    // With binary semaphores, we must add an execution dependency because signal and wait must be submitted in order.
                    submitNode.dependents.insert(&waitNode);
                    waitNode.dependencies.insert(&submitNode);
                }
            }
        }

        // TODO: Optimize resources and synchronization.

        // Compile resource providers.
        for (const auto& provider : graph->resources.indices) provider->createResources();
        for (const auto& provider : graph->resources.commandBuffers) provider->createResources();
        for (const auto& provider : graph->resources.semaphores) provider->createResources();
        for (const auto& provider : graph->resources.timelineSemaphores) provider->createResources();

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
                node->dependents.insert(finalNode.get());
                finalNode->dependencies.insert(node.get());
            }
        }
        graph->nodes.emplace_back(std::move(finalNode));

        return graph;
    }
}  // namespace sol
