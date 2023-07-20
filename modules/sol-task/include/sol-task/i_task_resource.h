#pragma once

namespace sol
{
    template<typename T>
    class ITaskResource
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITaskResource() = default;

        ITaskResource(const ITaskResource&) = default;

        ITaskResource(ITaskResource&&) noexcept = default;

        virtual ~ITaskResource() noexcept = default;

        ITaskResource& operator=(const ITaskResource&) = default;

        ITaskResource& operator=(ITaskResource&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual bool empty() const noexcept = 0;

        [[nodiscard]] virtual T& operator*() = 0;

        [[nodiscard]] virtual const T& operator*() const = 0;

        [[nodiscard]] virtual T* operator->() = 0;

        [[nodiscard]] virtual const T* operator->() const = 0;
    };
}  // namespace sol
