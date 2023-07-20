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
class FunctionTaskResourceList2 final : public sol::ITaskResourceList<T>
{
public:
    FunctionTaskResourceList2() = default;

    FunctionTaskResourceList2(const FunctionTaskResourceList2&) = default;

    FunctionTaskResourceList2(FunctionTaskResourceList2&&) noexcept = default;

    template<typename U>
    FunctionTaskResourceList2(U& res, std::function<bool(size_t)> f) : test(std::move(f))
    {
        for (auto& r : res) { resources.emplace_back(r); }
    }

    ~FunctionTaskResourceList2() noexcept override = default;

    FunctionTaskResourceList2& operator=(const FunctionTaskResourceList2&) = default;

    FunctionTaskResourceList2& operator=(FunctionTaskResourceList2&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override { return resources.empty() || !test; }

    [[nodiscard]] std::vector<T> get() override
    {
        if (empty()) return {};
        std::vector<T> res;
        for (size_t i = 0; i < resources.size(); i++)
        {
            if (test(i)) res.emplace_back(resources[i]);
        }
        return res;
    }

    [[nodiscard]] const std::vector<T> get() const override
    {
        if (empty()) return {};
        std::vector<T> res;
        for (size_t i = 0; i < resources.size(); i++)
        {
            if (test(i)) res.emplace_back(resources[i]);
        }
        return res;
    }

private:
    std::vector<T>              resources;
    std::function<bool(size_t)> test;
};