#include "pathtracing/command_pool.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-memory/memory_manager.h"

void createCommandPool(GlobalState& state)
{
    sol::VulkanCommandPool::Settings commandPoolSettings;
    commandPoolSettings.device           = *state.device;
    commandPoolSettings.queueFamilyIndex = state.memoryManager->getGraphicsQueue().getFamily().getIndex();
    commandPoolSettings.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    state.commandPool                    = sol::VulkanCommandPool::create(commandPoolSettings);

    sol::VulkanCommandBufferList::Settings commandBufferSettings;
    commandBufferSettings.commandPool = *state.commandPool;
    commandBufferSettings.size        = state.swapchain->getImageCount();
    commandBufferSettings.size        = state.maxFrames;
    state.guiCommandBuffers           = sol::VulkanCommandBufferList::create(commandBufferSettings);
    commandBufferSettings.size        = state.maxFrames;
    state.viewerCommandBuffers        = sol::VulkanCommandBufferList::create(commandBufferSettings);
    commandBufferSettings.size        = state.swapchain->getImageCount();
    state.displayCommandBuffers       = sol::VulkanCommandBufferList::create(commandBufferSettings);
}
