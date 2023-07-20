#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <stdexcept>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource.h"

template<typename T, bool Pointer>
class PointerTaskResource final : public sol::ITaskResource<T>
{
public:
    PointerTaskResource() = default;

    PointerTaskResource(const PointerTaskResource&) = default;

    PointerTaskResource(PointerTaskResource&&) noexcept = default;

    explicit PointerTaskResource(std::conditional_t<Pointer, std::unique_ptr<T>, T> res) : resource(std::move(res)) {}

    ~PointerTaskResource() noexcept override = default;

    PointerTaskResource& operator=(const PointerTaskResource&) = default;

    PointerTaskResource& operator=(PointerTaskResource&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept override
    {
        if constexpr (Pointer) return !resource;
        return false;
    }

    [[nodiscard]] T& operator*() override
    {
        if (empty()) throw std::runtime_error("");
        if constexpr (Pointer)
            return *resource;
        else
            return resource;
    }

    [[nodiscard]] const T& operator*() const override
    {
        if (empty()) throw std::runtime_error("");
        if constexpr (Pointer)
            return *resource;
        else
            return resource;
    }

    [[nodiscard]] T* operator->() override
    {
        if (empty()) throw std::runtime_error("");
        if constexpr (Pointer)
            return resource.get();
        else
            return &resource;
    }

    [[nodiscard]] const T* operator->() const override
    {
        if (empty()) throw std::runtime_error("");
        if constexpr (Pointer)
            return resource.get();
        else
            return &resource;
    }

    template<typename... Args>
    [[nodiscard]] static PointerTaskResource create(Args&&... args)
    {
        return PointerTaskResource(std::make_unique<T>(std::forward<Args>(args)...));
    }

    std::conditional_t<Pointer, std::unique_ptr<T>, T> resource;
};
