#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cstdint>

namespace sol
{
    class Version
    {
    public:
        uint32_t major = 1;
        uint32_t minor = 0;
        uint32_t patch = 0;

        [[nodiscard]] uint32_t get() const noexcept;

        [[nodiscard]] static uint32_t getApiVersion() noexcept;

        [[nodiscard]] static uint32_t getEngineVersion() noexcept;
    };
}  // namespace sol