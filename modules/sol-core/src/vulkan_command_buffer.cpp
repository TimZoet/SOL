#include "sol-core/vulkan_command_buffer.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanCommandBuffer::VulkanCommandBuffer(const Settings& set, const VkCommandBuffer vkCommandBuffer) :
        settings(set), commandBuffer(vkCommandBuffer)
    {
    }
#else
    VulkanCommandBuffer::VulkanCommandBuffer(const Settings& set, const VkCommandBuffer vkCommandBuffer) :
        commandPool(&set.commandPool()), commandBuffer(vkCommandBuffer)
    {
    }
#endif

    VulkanCommandBuffer::~VulkanCommandBuffer() noexcept
    {
        vkFreeCommandBuffers(getDevice().get(), getCommandPool().get(), 1, &commandBuffer);
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanCommandBufferPtr VulkanCommandBuffer::create(const Settings& settings)
    {
        const auto buffers = createImpl(settings, 1);
        return std::make_unique<VulkanCommandBuffer>(settings, buffers[0]);
    }

    std::vector<VulkanCommandBufferPtr> VulkanCommandBuffer::create(const Settings& settings, const uint32_t count)
    {
        return createImpl(settings, count) | std::views::transform([&settings](const auto b) {
                   return std::make_unique<VulkanCommandBuffer>(settings, b);
               }) |
               std::ranges::to<std::vector>();
    }

    VulkanCommandBufferSharedPtr VulkanCommandBuffer::createShared(const Settings& settings)
    {
        const auto buffers = createImpl(settings, 1);
        return std::make_shared<VulkanCommandBuffer>(settings, buffers[0]);
    }

    std::vector<VkCommandBuffer> VulkanCommandBuffer::createImpl(const Settings& settings, const uint32_t count)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = settings.commandPool;
        allocInfo.level              = settings.level;
        allocInfo.commandBufferCount = count;

        // Allocate buffers.
        std::vector<VkCommandBuffer> buffers(count, VK_NULL_HANDLE);
        handleVulkanError(
          vkAllocateCommandBuffers(settings.commandPool().getDevice().get(), &allocInfo, buffers.data()));

        return buffers;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanCommandBuffer::Settings& VulkanCommandBuffer::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanCommandBuffer::getDevice() noexcept { return getCommandPool().getDevice(); }

    const VulkanDevice& VulkanCommandBuffer::getDevice() const noexcept { return getCommandPool().getDevice(); }

    VulkanCommandPool& VulkanCommandBuffer::getCommandPool() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.commandPool();
#else
        return *commandPool;
#endif
    }

    const VulkanCommandPool& VulkanCommandBuffer::getCommandPool() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.commandPool();
#else
        return *commandPool;
#endif
    }

    const VkCommandBuffer& VulkanCommandBuffer::get() const noexcept { return commandBuffer; }

    ////////////////////////////////////////////////////////////////
    // Commands.
    ////////////////////////////////////////////////////////////////

    void VulkanCommandBuffer::beginCommand() const
    {
        VkCommandBufferBeginInfo cmdBufBeginInfo{};
        cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        handleVulkanError(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));
    }

    void VulkanCommandBuffer::beginOneTimeCommand() const
    {
        VkCommandBufferBeginInfo cmdBufBeginInfo{};
        cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        handleVulkanError(vkBeginCommandBuffer(commandBuffer, &cmdBufBeginInfo));
    }

    void VulkanCommandBuffer::endCommand() const { handleVulkanError(vkEndCommandBuffer(commandBuffer)); }

    void VulkanCommandBuffer::resetCommand(const VkCommandBufferResetFlags flags) const
    {
        handleVulkanError(vkResetCommandBuffer(commandBuffer, flags));
    }
}  // namespace sol
