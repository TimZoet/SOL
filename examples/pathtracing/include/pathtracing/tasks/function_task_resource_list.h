#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource_list.h"

template<typename T>
class FunctionTaskResourceList final : public sol::ITaskResourceList<T*>
{
public:
    FunctionTaskResourceList() = default;

    FunctionTaskResourceList(const FunctionTaskResourceList&) = default;

    FunctionTaskResourceList(FunctionTaskResourceList&&) noexcept = default;

    template<typename U>
    FunctionTaskResourceList(U& res, std::function<bool(size_t)> f) : test(std::move(f))
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

    ~FunctionTaskResourceList() noexcept override = default;

    FunctionTaskResourceList& operator=(const FunctionTaskResourceList&) = default;

    FunctionTaskResourceList& operator=(FunctionTaskResourceList&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override { return resources.empty() || !test; }

    [[nodiscard]] std::vector<T*> get() override
    {
        if (empty()) return {};
        std::vector<T*> res;
        for (size_t i = 0; i < resources.size(); i++)
        {
            if (test(i)) res.emplace_back(resources[i]);
        }
        return res;
    }

    [[nodiscard]] const std::vector<T*> get() const override
    {
        if (empty()) return {};
        std::vector<T*> res;
        for (size_t i = 0; i < resources.size(); i++)
        {
            if (test(i)) res.emplace_back(resources[i]);
        }
        return res;
    }

private:
    std::vector<T*>             resources;
    std::function<bool(size_t)> test;
};