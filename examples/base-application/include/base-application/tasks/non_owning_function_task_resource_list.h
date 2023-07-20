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
class NonOwningFunctionTaskResourceList final : public sol::ITaskResourceList<T>
{
public:
    using function_t = std::function<bool(T&, size_t)>;

    struct FunctionIterator : sol::ITaskResourceList<T>::IteratorImpl
    {
        using iterator_t = typename std::vector<T*>::iterator;
        function_t* f;
        iterator_t  it0, it1;
        size_t      index = 0;

        FunctionIterator(function_t* _f, iterator_t _it0, iterator_t _it1) : f(_f), it0(_it0), it1(_it1)
        {
            // Need to skip invalid values on construction.
            while (it0 + index < it1 && !f->operator()(**this, index))
                index++;
        }

        ~FunctionIterator() noexcept override = default;

        std::unique_ptr<typename sol::ITaskResourceList<T>::IteratorImpl> clone() override
        {
            return std::make_unique<FunctionIterator>(f, it0, it1);
        }

        void increment() override
        {
            // Increment until we reach a valid value or reach the end.
            do {
                index++;
            } while (it0 + index < it1 && !f->operator()(**this, index));
        }

        void add(const int32_t i) override
        {
            // Increment until we reach a valid value or reach the end.
            do {
                index++;
            } while (it0 + index < it1 && !f->operator()(**this, index));
        }

        T& operator*() const override { return **(it0 + index); }

        bool operator==(const typename sol::ITaskResourceList<T>::IteratorImpl& other) const override
        {
            return it0 + index >= static_cast<const FunctionIterator&>(other).it1;
        }
    };

    NonOwningFunctionTaskResourceList() = default;

    NonOwningFunctionTaskResourceList(const NonOwningFunctionTaskResourceList&) = default;

    NonOwningFunctionTaskResourceList(NonOwningFunctionTaskResourceList&&) noexcept = default;

    ~NonOwningFunctionTaskResourceList() noexcept override = default;

    NonOwningFunctionTaskResourceList& operator=(const NonOwningFunctionTaskResourceList&) = default;

    NonOwningFunctionTaskResourceList& operator=(NonOwningFunctionTaskResourceList&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] bool empty() const noexcept override { return resources.empty(); }

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

    std::vector<T*> resources;

    function_t test;
};