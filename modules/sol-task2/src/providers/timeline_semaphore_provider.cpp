#include "sol-task/providers/timeline_semaphore_provider.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_timeline_semaphore.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/compiled_graph.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TimelineSemaphoreProvider::TimelineSemaphoreProvider(CompiledGraph& g, const uint32_t count) :
        IProvider(g), index(g, count)
    {
    }

    TimelineSemaphoreProvider::~TimelineSemaphoreProvider() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    uint32_t TimelineSemaphoreProvider::getIndex() const { return index.getValue(); }

    std::pair<VulkanTimelineSemaphore*, uint64_t> TimelineSemaphoreProvider::getSemaphore(const bool increment)
    {
        if (increment) counters[getIndex()]++;
        return {semaphores[getIndex()].get(), counters[getIndex()]};
    }

    ////////////////////////////////////////////////////////////////
    // Graph setup.
    ////////////////////////////////////////////////////////////////

    void TimelineSemaphoreProvider::createResources()
    {
        const VulkanTimelineSemaphore::Settings settings{.device = getGraph().getDevice(), .initialValue = 0};
        semaphores = VulkanTimelineSemaphore::create(settings, index.getRange());
        counters.resize(index.getRange(), 0);
    }

    void TimelineSemaphoreProvider::loop() { index.increment(); }
}  // namespace sol
