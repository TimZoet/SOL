#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-task/i_task_resource_list.h"

template<std::integral I>
class IndexedCommandBufferListResourceList final : public sol::ITaskResourceList<VkCommandBuffer>
{
public:
    using index_t = I;

    IndexedCommandBufferListResourceList() = default;

    IndexedCommandBufferListResourceList(const IndexedCommandBufferListResourceList&) = default;

    IndexedCommandBufferListResourceList(IndexedCommandBufferListResourceList&&) noexcept = default;

    IndexedCommandBufferListResourceList(sol::VulkanCommandBufferList& res, index_t& i) : resource(&res), index(&i) {}

    ~IndexedCommandBufferListResourceList() noexcept override = default;

    IndexedCommandBufferListResourceList& operator=(const IndexedCommandBufferListResourceList&) = default;

    IndexedCommandBufferListResourceList& operator=(IndexedCommandBufferListResourceList&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override { return !resource; }

    [[nodiscard]] std::vector<VkCommandBuffer> get() override
    {
        if (empty()) return {};
        if (!index) throw std::runtime_error("");
        return {resource->get(*index)};
    }

    [[nodiscard]] const std::vector<VkCommandBuffer> get() const override
    {
        if (empty()) return {};
        if (!index) throw std::runtime_error("");
        return {resource->get(*index)};
    }

private:
    sol::VulkanCommandBufferList* resource = nullptr;
    index_t*                      index    = nullptr;
};