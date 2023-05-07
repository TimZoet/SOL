#include "sol-core/vulkan_command_buffer_list.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"

namespace
{
    void checkIndex(const size_t index, const size_t size)
    {
        if (index >= size) throw sol::SolError("Command buffer index is out of range.");
    }
}  // namespace

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    VulkanCommandBufferList::VulkanCommandBufferList(const Settings& set, std::vector<VkCommandBuffer> commandBuffers) :
        settings(set), buffers(std::move(commandBuffers))
    {
    }
#else
    VulkanCommandBufferList::VulkanCommandBufferList(const Settings& set, std::vector<VkCommandBuffer> commandBuffers) :
        commandPool(&set.commandPool()), buffers(std::move(commandBuffers))
    {
    }
#endif

    VulkanCommandBufferList::~VulkanCommandBufferList() noexcept
    {
        vkFreeCommandBuffers(
          getDevice().get(), getCommandPool().get(), static_cast<uint32_t>(buffers.size()), buffers.data());
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanCommandBufferListPtr VulkanCommandBufferList::create(const Settings& settings)
    {
        auto buffers = createImpl(settings);
        return std::make_unique<VulkanCommandBufferList>(settings, std::move(buffers));
    }

    VulkanCommandBufferListSharedPtr VulkanCommandBufferList::createShared(const Settings& settings)
    {
        auto buffers = createImpl(settings);
        return std::make_shared<VulkanCommandBufferList>(settings, std::move(buffers));
    }

    std::vector<VkCommandBuffer> VulkanCommandBufferList::createImpl(const Settings& settings)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = settings.commandPool;
        allocInfo.level              = settings.level;
        allocInfo.commandBufferCount = static_cast<uint32_t>(settings.size);

        // Allocate buffers.
        std::vector<VkCommandBuffer> buffers(settings.size, VK_NULL_HANDLE);
        handleVulkanError(
          vkAllocateCommandBuffers(settings.commandPool().getDevice().get(), &allocInfo, buffers.data()));

        return buffers;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
    const VulkanCommandBufferList::Settings& VulkanCommandBufferList::getSettings() const noexcept { return settings; }
#endif

    VulkanDevice& VulkanCommandBufferList::getDevice() noexcept { return getCommandPool().getDevice(); }

    const VulkanDevice& VulkanCommandBufferList::getDevice() const noexcept { return getCommandPool().getDevice(); }

    VulkanCommandPool& VulkanCommandBufferList::getCommandPool() noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.commandPool();
#else
        return *commandPool;
#endif
    }

    const VulkanCommandPool& VulkanCommandBufferList::getCommandPool() const noexcept
    {
#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        return settings.commandPool();
#else
        return *commandPool;
#endif
    }

    size_t VulkanCommandBufferList::getSize() const noexcept { return buffers.size(); }

    const VkCommandBuffer& VulkanCommandBufferList::get(const size_t index) const
    {
        checkIndex(index, buffers.size());
        return buffers[index];
    }

    ////////////////////////////////////////////////////////////////
    // Commands.
    ////////////////////////////////////////////////////////////////

    void VulkanCommandBufferList::beginCommand(const size_t index) const
    {
        checkIndex(index, buffers.size());
        VkCommandBufferBeginInfo cmdBufBeginInfo{};
        cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        handleVulkanError(vkBeginCommandBuffer(buffers[index], &cmdBufBeginInfo));
    }

    void VulkanCommandBufferList::beginOneTimeCommand(const size_t index) const
    {
        checkIndex(index, buffers.size());
        VkCommandBufferBeginInfo cmdBufBeginInfo{};
        cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        handleVulkanError(vkBeginCommandBuffer(buffers[index], &cmdBufBeginInfo));
    }

    void VulkanCommandBufferList::endCommand(const size_t index) const
    {
        checkIndex(index, buffers.size());
        handleVulkanError(vkEndCommandBuffer(buffers[index]));
    }

    void VulkanCommandBufferList::resetCommand(const size_t index, const VkCommandBufferResetFlags flags) const
    {
        checkIndex(index, buffers.size());
        handleVulkanError(vkResetCommandBuffer(buffers[index], flags));
    }
}  // namespace sol