#pragma once

namespace sol
{
    template<typename T>
    class ITaskResourceList
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using element_t = T;

        struct IteratorImpl
        {
            virtual ~IteratorImpl() noexcept                                                  = default;
            virtual std::unique_ptr<IteratorImpl> clone()                                     = 0;
            virtual void                          increment()                                 = 0;
            virtual void                          add(int32_t i)                              = 0;
            virtual T&                            operator*() const                           = 0;
            virtual bool                          operator==(const IteratorImpl& other) const = 0;
        };

        struct Iterator
        {
            std::unique_ptr<IteratorImpl> impl;

            using difference_type   = long;
            using value_type        = T;
            using pointer           = T*;
            using reference         = T&;
            using iterator_category = std::input_iterator_tag;

            Iterator() = default;

            Iterator(const Iterator& other) : impl(other.impl->clone()) {}

            Iterator(Iterator&&) noexcept = default;

            explicit Iterator(std::unique_ptr<IteratorImpl> impl) : impl(std::move(impl)) {}

            ~Iterator() noexcept = default;

            Iterator& operator=(const Iterator& other)
            {
                impl(other.impl->clone());
                return *this;
            }

            Iterator& operator=(Iterator&&) noexcept = default;

            auto& operator++()
            {
                impl->increment();
                return *this;
            }

            auto operator++(int32_t i)
            {
                impl->add(i);
                return *this;
            }

            bool operator==(const Iterator& other) const { return *impl == *other.impl; }

            bool operator!=(const Iterator& other) const { return !(*this == other); }

            reference operator*() const { return **impl; }
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITaskResourceList() = default;

        ITaskResourceList(const ITaskResourceList&) = default;

        ITaskResourceList(ITaskResourceList&&) noexcept = default;

        virtual ~ITaskResourceList() noexcept = default;

        ITaskResourceList& operator=(const ITaskResourceList&) = default;

        ITaskResourceList& operator=(ITaskResourceList&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual bool empty() const noexcept = 0;

        [[nodiscard]] virtual Iterator begin() noexcept = 0;

        [[nodiscard]] virtual Iterator end() noexcept = 0;
    };
}  // namespace sol
