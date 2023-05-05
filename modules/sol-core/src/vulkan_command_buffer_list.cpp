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

    VulkanCommandBufferList::VulkanCommandBufferList(SettingsPtr                  settingsPtr,
                                                     std::vector<VkCommandBuffer> commandBuffers) :
        settings(std::move(settingsPtr)), buffers(std::move(commandBuffers))
    {
    }

    VulkanCommandBufferList::~VulkanCommandBufferList() noexcept
    {
        vkFreeCommandBuffers(getDevice().get(),
                             settings->commandPool,
                             static_cast<uint32_t>(buffers.size()),
                             buffers.data());
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    VulkanCommandBufferListPtr VulkanCommandBufferList::create(Settings settings)
    {
        auto buffers = createImpl(settings);
        return std::make_unique<VulkanCommandBufferList>(std::make_unique<Settings>(settings), std::move(buffers));
    }

    VulkanCommandBufferListSharedPtr VulkanCommandBufferList::createShared(Settings settings)
    {
        auto buffers = createImpl(settings);
        return std::make_shared<VulkanCommandBufferList>(std::make_unique<Settings>(settings), std::move(buffers));
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

    const VulkanCommandBufferList::Settings& VulkanCommandBufferList::getSettings() const noexcept { return *settings; }

    VulkanDevice& VulkanCommandBufferList::getDevice() noexcept { return settings->commandPool().getDevice(); }

    const VulkanDevice& VulkanCommandBufferList::getDevice() const noexcept
    {
        return settings->commandPool().getDevice();
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