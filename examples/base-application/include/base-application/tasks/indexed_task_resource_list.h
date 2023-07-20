#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-task/i_task_resource_list.h"

template<typename T, bool Pointer, std::integral I = uint32_t>
class IndexedTaskResourceList final : public sol::ITaskResourceList<T>
{
public:
    using index_t = I;

    struct IndexedIterator : sol::ITaskResourceList<T>::IteratorImpl
    {
        using iterator_t = typename std::vector<std::conditional_t<Pointer, std::unique_ptr<T>, T>>::iterator;
        iterator_t it;

        IndexedIterator(iterator_t _it) : it(_it) {}

        ~IndexedIterator() noexcept override = default;

        std::unique_ptr<typename sol::ITaskResourceList<T>::IteratorImpl> clone() override
        {
            return std::make_unique<IndexedIterator>(it);
        }

        void increment() override { ++it; }

        void add(int32_t i) override { it += i; }

        T& operator*() const override
        {
            if constexpr (Pointer)
                return **it;
            else
                return *it;
        }

        bool operator==(const typename sol::ITaskResourceList<T>::IteratorImpl& other) const override { return it == static_cast<const IndexedIterator&>(other).it; }
    };

    IndexedTaskResourceList() = default;

    IndexedTaskResourceList(const IndexedTaskResourceList&) = default;

    IndexedTaskResourceList(IndexedTaskResourceList&&) noexcept = default;

    ~IndexedTaskResourceList() noexcept override = default;

    IndexedTaskResourceList& operator=(const IndexedTaskResourceList&) = default;

    IndexedTaskResourceList& operator=(IndexedTaskResourceList&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] bool empty() const noexcept override { return resources.empty(); }

    [[nodiscard]] typename sol::ITaskResourceList<T>::Iterator begin() noexcept override
    {
        return sol::ITaskResourceList<T>::Iterator(std::make_unique<IndexedIterator>(resources.begin() + *index));
    }

    [[nodiscard]] typename sol::ITaskResourceList<T>::Iterator end() noexcept override
    {
        return sol::ITaskResourceList<T>::Iterator(std::make_unique<IndexedIterator>(resources.begin() + *index + 1));
    }

    std::vector<std::conditional_t<Pointer, std::unique_ptr<T>, T>> resources;

    index_t* index = nullptr;
};