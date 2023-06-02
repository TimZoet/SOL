#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

namespace sol
{
    class RenderSettings
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class ClearColorFormat
        {
            Float,
            Int,
            Uint
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RenderSettings();

        RenderSettings(const RenderSettings&) = delete;

        RenderSettings(RenderSettings&&) = delete;

        ~RenderSettings() noexcept;

        RenderSettings& operator=(const RenderSettings&) = delete;

        RenderSettings& operator=(RenderSettings&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] ClearColorFormat getClearColorFormat() const noexcept;

        [[nodiscard]] std::array<float, 4> getClearColorFloat() const noexcept;

        [[nodiscard]] std::array<int32_t, 4> getClearColorInt() const noexcept;

        [[nodiscard]] std::array<uint32_t, 4> getClearColorUint() const noexcept;

        [[nodiscard]] float getClearDepth() const noexcept;

        [[nodiscard]] uint32_t getClearStencil() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setClearColorFormat(ClearColorFormat format) noexcept;

        void setClearColorFloat(std::array<float, 4> value) noexcept;

        void setClearColorInt(std::array<int32_t, 4> value) noexcept;

        void setClearColorUint(std::array<uint32_t, 4> value) noexcept;

        void setClearDepth(float value) noexcept;

        void setClearStencil(uint32_t value) noexcept;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ClearColorFormat        clearColorFormat = ClearColorFormat::Float;
        std::array<float, 4>    clearColorFloat  = {0, 0, 0, 0};
        std::array<int32_t, 4>  clearColorInt    = {0, 0, 0, 0};
        std::array<uint32_t, 4> clearColorUint   = {0, 0, 0, 0};
        float                   clearDepth       = 1.0f;
        uint32_t                clearStencil     = 0;
    };
}  // namespace sol
