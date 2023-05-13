#include "sol-command/other/fence_command.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-command/command_queue.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    FenceCommand::FenceCommand() = default;

    FenceCommand::~FenceCommand() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    FenceCommand::Action FenceCommand::getAction() const noexcept { return action; }

    const std::vector<VulkanFence*>& FenceCommand::getFences() const noexcept { return fences; }

    const uint32_t* FenceCommand::getFenceIndexPtr() const noexcept { return fenceIndexPtr; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void FenceCommand::setAction(const Action a)
    {
        commandQueue->requireNonFinalized();
        action = a;
    }

    void FenceCommand::addFence(VulkanFence& fence)
    {
        commandQueue->requireNonFinalized();
        fences.emplace_back(&fence);
    }

    void FenceCommand::setFenceIndexPtr(uint32_t* ptr)
    {
        commandQueue->requireNonFinalized();
        fenceIndexPtr = ptr;
    }

    ////////////////////////////////////////////////////////////////
    // Run.
    ////////////////////////////////////////////////////////////////

    void FenceCommand::finalize()
    {
        if (fences.empty()) throw SolError("Cannot finalize FenceCommand: no fences added.");
    }

    void FenceCommand::operator()()
    {
        VkDevice device;

        // Get handles.
        std::vector<VkFence> fenceHandles;
        if (fenceIndexPtr)
        {
            if (*fenceIndexPtr >= fences.size())
                throw SolError("Cannot run FenceCommand: fenceIndexPtr is out of bounds.");

            const auto& fence = *fences[*fenceIndexPtr];
            fenceHandles.emplace_back(fence.get());
            device = fence.getSettings().device;
        }
        else
        {
            fenceHandles.reserve(fences.size());
            std::ranges::transform(fences, std::back_inserter(fenceHandles), [&](const auto& f) { return f->get(); });
            device = fences.front()->getDevice().get();
        }

        // Wait for fences.
        if (any(action & Action::Wait))
            handleVulkanError(vkWaitForFences(
              device, static_cast<uint32_t>(fenceHandles.size()), fenceHandles.data(), VK_TRUE, UINT64_MAX));

        // Reset fences.
        if (any(action & Action::Reset))
            handleVulkanError(vkResetFences(device, static_cast<uint32_t>(fenceHandles.size()), fenceHandles.data()));
    }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    std::string FenceCommand::getVizLabel() const { return "Fence"; }
}  // namespace sol
