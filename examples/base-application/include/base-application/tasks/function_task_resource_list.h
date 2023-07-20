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

template<typename T, bool Pointer>
class FunctionTaskResourceList final : public sol::ITaskResourceList<T>
{
public:
    using function_t = std::function<bool(T&, size_t)>;

    struct FunctionIterator : sol::ITaskResourceList<T>::IteratorImpl
    {
        using iterator_t = typename std::vector<std::conditional_t<Pointer, std::unique_ptr<T>, T>>::iterator;
        function_t* f;
        iterator_t  it0, it1;
        size_t      index = 0;

        FunctionIterator(function_t* _f, iterator_t _it0, iterator_t _it1) : f(_f), it0(_it0), it1(_it1)
        {
            while (it0 + index < it1 && !f->operator()(**this, index)) index++;
        }

        ~FunctionIterator() noexcept override = default;

        std::unique_ptr<typename sol::ITaskResourceList<T>::IteratorImpl> clone() override
        {
            return std::make_unique<FunctionIterator>(f, it0, it1);
        }

        void increment() override
        {
            do {
                index++;
            } while (it0 + index < it1 && !f->operator()(**this, index));
        }

        void add(const int32_t i) override
        {
            do {
                index++;
            } while (it0 + index < it1 && !f->operator()(**this, index));
        }

        T& operator*() const override
        {
            if constexpr (Pointer)
                return **(it0 + index);
            else
                return *(it0 + index);
        }

        bool operator==(const typename sol::ITaskResourceList<T>::IteratorImpl& other) const override
        {
            return it0 + index >= static_cast<const FunctionIterator&>(other).it1;
        }
    };

    FunctionTaskResourceList() = default;

    FunctionTaskResourceList(const FunctionTaskResourceList&) = default;

    FunctionTaskResourceList(FunctionTaskResourceList&&) noexcept = default;

    ~FunctionTaskResourceList() noexcept override = default;

    FunctionTaskResourceList& operator=(const FunctionTaskResourceList&) = default;

    FunctionTaskResourceList& operator=(FunctionTaskResourceList&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] bool empty() const noexcept override { return resources.empty() || !test; }

    [[nodiscard]] typename sol::ITaskResourceList<T>::Iterator begin() noexcept override
    {
        return sol::ITaskResourceList<T>::Iterator(
          std::make_unique<FunctionIterator>(&test, resources.begin(), resources.end()));
    }

    [[nodiscard]] typename sol::ITaskResourceList<T>::Iterator end() noexcept override
    {
        return sol::ITaskResourceList<T>::Iterator(
          std::make_unique<FunctionIterator>(&test, resources.begin(), resources.end()));
    }

    std::vector<std::conditional_t<Pointer, std::unique_ptr<T>, T>> resources;

    function_t test;
};
