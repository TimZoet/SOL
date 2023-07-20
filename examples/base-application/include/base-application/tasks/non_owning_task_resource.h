#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <stdexcept>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

template<typename T>
class NonOwningTaskResource final : public sol::ITaskResource<T>
{
public:
    NonOwningTaskResource() = default;

    NonOwningTaskResource(const NonOwningTaskResource&) = default;

    NonOwningTaskResource(NonOwningTaskResource&&) noexcept = default;

    explicit NonOwningTaskResource(T& res) : resource(&res) {}

    ~NonOwningTaskResource() noexcept override = default;

    NonOwningTaskResource& operator=(const NonOwningTaskResource&) = default;

    NonOwningTaskResource& operator=(NonOwningTaskResource&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override { return !resource; }

    [[nodiscard]] T& operator*() override
    {
        if (empty()) throw std::runtime_error("");
        return *resource;
    }

    [[nodiscard]] const T& operator*() const override
    {
        if (empty()) throw std::runtime_error("");
        return *resource;
    }

    [[nodiscard]] T* operator->() override
    {
        if (empty()) throw std::runtime_error("");
        return resource;
    }

    [[nodiscard]] const T* operator->() const override
    {
        if (empty()) throw std::runtime_error("");
        return resource;
    }

    T* resource = nullptr;
};
