#include "pathtracing/sync_objects.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_swapchain.h"

void createSyncObjects(GlobalState& state)
{
    state.displayInUseFences.resize(state.swapchain->getImageCount(), nullptr);
    {
        sol::VulkanFence::Settings fenceSettings;
        fenceSettings.device   = state.device;
        fenceSettings.signaled = true;
        sol::VulkanSemaphore::Settings semSettings;
        semSettings.device = state.device;

        for (size_t i = 0; i < state.maxFrames; i++)
        {
            state.guiCBInUseFences.emplace_back(sol::VulkanFence::create(fenceSettings));
            state.guiRenderFinishedSemaphores.emplace_back(sol::VulkanSemaphore::create(semSettings));
            state.viewerCBInUseFences.emplace_back(sol::VulkanFence::create(fenceSettings));
            state.viewerRenderFinishedSemaphores.emplace_back(sol::VulkanSemaphore::create(semSettings));
            state.displayCBInUseFences.emplace_back(sol::VulkanFence::create(fenceSettings));
            state.displayRenderFinishedSemaphores.emplace_back(sol::VulkanSemaphore::create(semSettings));
            state.displayimageAvailableSemaphores.emplace_back(sol::VulkanSemaphore::create(semSettings));
        }
    }
}
