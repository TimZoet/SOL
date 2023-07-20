#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <stdexcept>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

template<typename T, std::integral I = uint32_t>
class IndexedTaskResource final : public sol::ITaskResource<T>
{
public:
    using index_t = I;

    IndexedTaskResource() = default;

    IndexedTaskResource(const IndexedTaskResource&) = default;

    IndexedTaskResource(IndexedTaskResource&&) noexcept = default;

    template<typename U>
    IndexedTaskResource(U& res, index_t& i) : index(&i)
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

    ~IndexedTaskResource() noexcept override = default;

    IndexedTaskResource& operator=(const IndexedTaskResource&) = default;

    IndexedTaskResource& operator=(IndexedTaskResource&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override { return resources.empty(); }

    [[nodiscard]] T& get() override
    {
        if (empty()) throw std::runtime_error("");
        if (!index) throw std::runtime_error("");
        return *resources[*index];
    }

    [[nodiscard]] const T& get() const override
    {
        if (empty()) throw std::runtime_error("");
        if (!index) throw std::runtime_error("");
        return *resources[*index];
    }

private:
    std::vector<T*> resources;
    index_t*        index = nullptr;
};
