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
class PointerTaskResource : public sol::ITaskResource<T>
{
public:
    PointerTaskResource() = default;

    PointerTaskResource(const PointerTaskResource&) = default;

    PointerTaskResource(PointerTaskResource&&) noexcept = default;

    explicit PointerTaskResource(T& res) : resource(&res) {}

    ~PointerTaskResource() noexcept override = default;

    PointerTaskResource& operator=(const PointerTaskResource&) = default;

    PointerTaskResource& operator=(PointerTaskResource&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override { return !resource; }

    [[nodiscard]] T& get() override
    {
        if (empty()) throw std::runtime_error("");
        return *resource;
    }

    [[nodiscard]] const T& get() const override
    {
        if (empty()) throw std::runtime_error("");
        return *resource;
    }

private:
    T* resource = nullptr;
};
