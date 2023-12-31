#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace sol
{
    std::vector<const char*> stringVectorToConstCharVector(const std::vector<std::string>& strings);

    uint32_t crc32(const uint8_t* data, size_t size);

    template<std::integral T>
    bool overlaps(std::array<T, 2> lower0, std::array<T, 2> upper0, std::array<T, 2> lower1, std::array<T, 2> upper1)
    {
        if (lower0[0] >= lower1[0] && lower0[1] >= lower1[1] && lower0[0] < upper1[0] && lower0[1] < upper1[1])
            return true;
        if (upper0[0] > lower1[0] && upper0[1] > lower1[1] && upper0[0] <= upper1[0] && upper0[1] <= upper1[1])
            return true;

        return false;
    }

    template<typename T>
    [[nodiscard]] std::vector<const T*> raw(const std::vector<std::unique_ptr<T>>& vec)
    {
        return vec | std::views::transform([](const auto& v) { return v.get(); }) |
               std::ranges::to<std::vector<const T*>>();
    }

    template<typename T>
    struct VectorUniquePtrIterator
    {
        using difference_type = std::ptrdiff_t;
        using element_type    = T;
        using pointer         = T*;
        using reference       = T&;

    private:
        const std::unique_ptr<T>* ptr = nullptr;

    public:
        VectorUniquePtrIterator() { throw std::runtime_error("Not implemented."); }

        explicit VectorUniquePtrIterator(const std::unique_ptr<T>* p) : ptr(p) {}

        reference operator*() const { return **ptr; }

        auto& operator++()
        {
            ptr++;
            return *this;
        }

        auto operator++(int)
        {
            const auto tmp = *this;
            ++*this;
            return tmp;
        }

        [[nodiscard]] auto operator<=>(const VectorUniquePtrIterator&) const = default;
    };
}  // namespace sol
