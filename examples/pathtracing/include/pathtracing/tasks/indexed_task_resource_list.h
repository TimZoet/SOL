#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource_list.h"

template<typename T, std::integral I>
class IndexedTaskResourceList final : public sol::ITaskResourceList<T*>
{
public:
    using index_t = I;

    IndexedTaskResourceList() = default;

    IndexedTaskResourceList(const IndexedTaskResourceList&) = default;

    IndexedTaskResourceList(IndexedTaskResourceList&&) noexcept = default;

    template<typename U>
    IndexedTaskResourceList(U& res, index_t& i) : index(&i)
    {
        for (auto& r : res)
        {
            if constexpr (std::is_pointer_v<std::decay_t<decltype(r)>>)
                resources.emplace_back(r);
            else if constexpr (std::same_as<std::decay_t<decltype(r)>, std::unique_ptr<T>>)
                resources.emplace_back(r.get());
            else
                resources.emplace_back(&r);
        }
    }

    ~IndexedTaskResourceList() noexcept override = default;

    IndexedTaskResourceList& operator=(const IndexedTaskResourceList&) = default;

    IndexedTaskResourceList& operator=(IndexedTaskResourceList&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override { return resources.empty(); }

    [[nodiscard]] std::vector<T*> get() override
    {
        if (empty()) return {};
        if (!index) throw std::runtime_error("");
        return {resources[*index]};
    }

    [[nodiscard]] const std::vector<T*> get() const override
    {
        if (empty()) return {};
        if (!index) throw std::runtime_error("");
        return {resources[*index]};
    }

private:
    std::vector<T*> resources;
    index_t*        index = nullptr;
};