#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"
#include "sol-task/providers/i_provider.h"
#include "sol-task/providers/index_provider.h"

namespace sol
{
    class TimelineSemaphoreProvider final : public IProvider
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TimelineSemaphoreProvider() = delete;

        TimelineSemaphoreProvider(CompiledGraph& g, uint32_t count);

        TimelineSemaphoreProvider(const TimelineSemaphoreProvider&) = delete;

        TimelineSemaphoreProvider(TimelineSemaphoreProvider&&) = delete;

        ~TimelineSemaphoreProvider() noexcept override;

        TimelineSemaphoreProvider& operator=(const TimelineSemaphoreProvider&) = delete;

        TimelineSemaphoreProvider& operator=(TimelineSemaphoreProvider&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] uint32_t getIndex() const;

        [[nodiscard]] std::pair<VulkanTimelineSemaphore*, uint64_t> getSemaphore(bool increment);

        ////////////////////////////////////////////////////////////////
        // Graph setup.
        ////////////////////////////////////////////////////////////////

        void createResources() override;

        void loop();

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IndexProvider index;

        std::vector<VulkanTimelineSemaphorePtr> semaphores;

        std::vector<uint64_t> counters;
    };
}  // namespace sol
