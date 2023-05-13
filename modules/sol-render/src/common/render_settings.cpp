#include "sol-render/common/render_settings.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RenderSettings::RenderSettings() = default;

    RenderSettings::~RenderSettings() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    RenderSettings::ClearColorFormat RenderSettings::getClearColorFormat() const noexcept { return clearColorFormat; }

    std::array<float, 4> RenderSettings::getClearColorFloat() const noexcept { return clearColorFloat; }

    std::array<int32_t, 4> RenderSettings::getClearColorInt() const noexcept { return clearColorInt; }

    std::array<uint32_t, 4> RenderSettings::getClearColorUint() const noexcept { return clearColorUint; }

    float RenderSettings::getClearDepth() const noexcept { return clearDepth; }

    uint32_t RenderSettings::getClearStencil() const noexcept { return clearStencil; }

    RenderSettings::CullMode RenderSettings::getCullMode() const noexcept { return cullMode; }

    RenderSettings::FrontFace RenderSettings::getFrontFace() const noexcept { return frontFace; }

    RenderSettings::PolygonMode RenderSettings::getPolyonMode() const noexcept { return polygonMode; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RenderSettings::setClearColorFormat(const ClearColorFormat format) noexcept { clearColorFormat = format; }

    void RenderSettings::setClearColorFloat(const std::array<float, 4> value) noexcept { clearColorFloat = value; }

    void RenderSettings::setClearColorInt(const std::array<int32_t, 4> value) noexcept { clearColorInt = value; }

    void RenderSettings::setClearColorUint(const std::array<uint32_t, 4> value) noexcept { clearColorUint = value; }

    void RenderSettings::setClearDepth(const float value) noexcept { clearDepth = value; }

    void RenderSettings::setClearStencil(const uint32_t value) noexcept { clearStencil = value; }

    void RenderSettings::setCullMode(const CullMode value) noexcept { cullMode = value; }

    void RenderSettings::setFrontFace(const FrontFace value) noexcept { frontFace = value; }

    void RenderSettings::setPolygonMode(PolygonMode value) noexcept { polygonMode = value; }
}  // namespace sol